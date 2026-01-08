// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include <platforminterface/allocator.h>
#include <platforminterface/log.h>
#include <platforminterface/screen.h>
#include <platform/devicelink.h>
#include <platform/platform.h>
#include <qul/private/application.h>
#include <qul/private/layers/layers.h>
#include <qul/private/input.h>
#include "qul/private/staticscreenrotationhelpers.h"

#ifndef QUL_PLATFORM_SKIP_PLATFORM_CONFIG
#include "platform_config.h"
#endif

#ifdef QUL_PLATFORM_DEVICELINK_ENABLED
#include <qul/private/devicelink.h>
#include "devicelinkdefs.h"
#include <platforminterface/platforminterface.h>

// ProtoBuf headers
#include <pb_encode.h>
#include <pb_decode.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "deviceinformationexchangeprotocol.pb.h"
#include "minihdlc.h"
#ifdef __cplusplus
}

#endif

#define QOI_IMPLEMENTATION
#define QOI_NO_STDIO
#include <qoi.h>

static bool hdlcEnabled = false;
static TouchEvent touchEvent = TouchEvent_init_default;
static uint16_t touchCallId = 0;
static KeyEvent keyEvent = KeyEvent_init_default;
static uint16_t keyCallId = 0;

static void SendCharWrapper(uint8_t data)
{
    Qul::Platform::getDeviceLinkInterface()->transmitChars(&data, 1);
}

namespace {

struct DeviceLinkFrameHeader
{
    uint8_t status;
    uint8_t pad;
    uint8_t index[2];

    uint16_t getIndex() const { return index[1] | uint16_t(index[0]) << 8; }

    void setIndex(uint16_t newIndex)
    {
        index[0] = (newIndex >> 8);
        index[1] = newIndex & 0xff;
    }
};
static_assert(sizeof(DeviceLinkFrameHeader) == 4, "Invalid size of the HDLC header");

union DeviceLinkFrame {
    DeviceLinkFrameHeader header;
    uint8_t data[MINIHDLC_MAX_FRAME_LENGTH];
};
static_assert(sizeof(DeviceLinkFrame) == MINIHDLC_MAX_FRAME_LENGTH, "Invalid size of the HDLC frame");

class HDLCOutgoingMessage
{
private:
    DeviceLinkFrame frame;
    size_t dataFill;
    uint16_t index;

    void sendFrame()
    {
        frame.header.setIndex(index);
        minihdlc_send_frame(&frame.data[0], dataFill);
        dataFill = sizeof(DeviceLinkFrameHeader);
        index++;
    }

public:
    HDLCOutgoingMessage()
        : frame{{0, 0, 0, 0}}
        , dataFill{sizeof(DeviceLinkFrameHeader)}
        , index{0}
    {}

    void finish()
    {
        frame.header.status = 1;
        sendFrame();
    }

    size_t writeData(const void *data, size_t size)
    {
        if (dataFill == sizeof(DeviceLinkFrame)) {
            sendFrame();
        }

        size_t chunkSize = std::min(size, sizeof(DeviceLinkFrame) - dataFill);
        memcpy(&frame.data[dataFill], data, chunkSize);
        dataFill += chunkSize;

        return chunkSize;
    }
};

bool writeMessageData(pb_ostream_t *stream, const pb_byte_t *buf, size_t count)
{
    HDLCOutgoingMessage *message = static_cast<HDLCOutgoingMessage *>(stream->state);
    while (count > 0) {
        size_t chunkSize = message->writeData(buf, count);
        count -= chunkSize;
        buf += chunkSize;
    }
    return true;
}

/*!
    \brief Called for a message to be transferred to the host

    When a protocol message is ready to be sent this function will use
    the HDLC interface to send frames over the serial port.
    \a message is the message to be sent.

    \internal
*/
void sendMessage(const DeviceToHostMessage *message)
{
    hdlcEnabled = true;
    HDLCOutgoingMessage hdlcMessage;

    /* Create a stream that will write to our buffer. */
#ifdef PB_BUFFER_ONLY
#error We need full support of buffer callbacks to implement variable-size messages
#endif
    pb_ostream_t stream{
        &writeMessageData,
        &hdlcMessage,
        (sizeof(DeviceLinkFrame) - 4) * 0x10000, // limit the overall message size
        0,
#ifndef PB_NO_ERRMSG
        nullptr,
#endif
    };

    /* Now we are ready to encode the message! */
    bool status = pb_encode(&stream, DeviceToHostMessage_fields, message);
    /* Check for errors... */
    if (!status) {
        Qul::PlatformInterface::log("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return;
    }

    hdlcMessage.finish();
}

static bool encodeString(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    const pb_byte_t *bstr = static_cast<const pb_byte_t *>(*arg);
    const char *cstr = static_cast<const char *>(*arg);
    return pb_encode_tag_for_field(stream, field) && pb_encode_string(stream, bstr, strlen(cstr));
}

static void sendError(uint32_t callId, ErrorCode errorCode, const char *errorMessage)
{
    DeviceToHostMessage reply = DeviceToHostMessage_init_zero;
    reply.replyId = callId;
    reply.which_alternative = DeviceToHostMessage_callDone_tag;
    reply.alternative.callDone.errorCode = errorCode;
    reply.alternative.callDone.which_alternative = CallDone_errorMessage_tag;
    reply.alternative.callDone.alternative.errorMessage.funcs.encode = &encodeString;
    reply.alternative.callDone.alternative.errorMessage.arg = const_cast<char *>(errorMessage);
    sendMessage(&reply);
}

static void sendSuccess(uint32_t callId)
{
    DeviceToHostMessage reply = DeviceToHostMessage_init_zero;
    reply.replyId = callId;
    reply.which_alternative = DeviceToHostMessage_callDone_tag;
    reply.alternative.callDone.errorCode = ErrorCode_Success;
    sendMessage(&reply);
}

static int findScreenId(Qul::Private::Items::ItemLayer *layer)
{
    using Qul::Private::Items::Screen;
    using PlatformScreen = Qul::PlatformInterface::Screen;

    size_t numPlatformScreens = 0;
    PlatformScreen *platformScreens = Qul::Platform::getPlatformInstance()->availableScreens(&numPlatformScreens);
    Screen *screen = layer->parent<Screen>();

    for (size_t i = 0; i < numPlatformScreens; ++i) {
        if (screen->node.platformScreen == &platformScreens[i]) {
            return i;
        }
    }
    return -1;
}

void handleGetFramebufferData(uint32_t callId, const GetFramebufferData &message)
{
    auto app = Qul::Private::Application::instance();
    if (!app)
        return;

    const Qul::PlatformInterface::LayerEngine::ItemLayer *platformLayer = NULL;
    Qul::Private::Items::ItemLayer *layer = NULL;
    if (Qul::Platform::getPlatformInstance()->layerEngine()) {
        layer = Qul::Private::Items::ItemLayer::asItemLayer(app->layerForPlatformId(message.platformId));
        if (layer) {
            platformLayer = layer->node.platformInstance();
            if (!platformLayer) {
                Qul::PlatformInterface::log("DeviceLink: The item layer with platform id %d is not instantiated, "
                                            "meaning it's not enabled or it hasn't been initialized yet\n",
                                            message.platformId);
                sendError(callId,
                          ErrorCode_GenericError,
                          "The item layer with specified platform id is not instantiated, "
                          "meaning it's not enabled or it hasn't been initialized yet");
                return;
            }
        } else {
            Qul::PlatformInterface::log("DeviceLink: No item layer found with the platform id %d\n", message.platformId);
            sendError(callId, ErrorCode_GenericError, "No item layer found with the specified platform id");
            return;
        }
    }

    auto format = Qul::Platform::getDeviceLinkInterface()->framebufferFormat(platformLayer);

    const uint32_t bytesPerPixel = format.bitsPerPixel / 8;

    // How many bytes to increment the address by when moving right one pixel in
    // the virtual coordinate system
    int32_t xIncrement = bytesPerPixel;

    // How many bytes to increment the address by when moving to the start of
    // the next line from the end of the previous line in the virtual coordinate system
    int32_t yIncrement = format.bytesPerLine - format.width * xIncrement;

    uint8_t *address = reinterpret_cast<uint8_t *>(format.address);

    if (Qul::Private::StaticScreenRotationHelpers::isDisplayRotationApplicable()) {
        const Qul::PlatformInterface::Size virtualSize = Qul::Private::StaticScreenRotationHelpers::rotatedSize(
            Qul::PlatformInterface::Size(format.width, format.height));
        const Qul::PlatformInterface::Point physicalOrigin
            = Qul::Private::StaticScreenRotationHelpers::mapVirtualToPhysical(virtualSize
                                                                                  - Qul::PlatformInterface::Size(1, 1),
                                                                              Qul::PlatformInterface::Point());

        format.width = virtualSize.width();
        format.height = virtualSize.height();

        // Shift address to the origin of the virtual coordinate system, since
        // that's where we want to start reading
        address += physicalOrigin.y() * format.bytesPerLine + physicalOrigin.x() * bytesPerPixel;

        switch (Qul::Private::displayRotationAngle) {
        case 90:
            xIncrement = format.bytesPerLine;
            yIncrement = -bytesPerPixel - format.width * xIncrement;
            break;
        case 180:
            xIncrement = -bytesPerPixel;
            yIncrement = -format.bytesPerLine - format.width * xIncrement;
            break;
        case 270:
            xIncrement = -format.bytesPerLine;
            yIncrement = bytesPerPixel - format.width * xIncrement;
            break;
        }
    }

    struct PixelData
    {
        uint32_t x;
        int32_t xIncrement;
        int32_t yIncrement;
        uint8_t *address;

        const Qul::Platform::FramebufferFormat &format;

        uint32_t operator()()
        {
            uint32_t pixelValue = 0;

            uint32_t index = 0;
            uint32_t remainingBits = 8;

            uint32_t num = format.bitsPerPixel;

            while (num > 0) {
                if (remainingBits == 0) {
                    remainingBits = 8;
                    ++index;
                }

                uint32_t count = std::min(num, remainingBits);
                pixelValue <<= count;

                int swappedIndex = index;
                if (format.swapBytes == 2) {
                    swappedIndex ^= 1;
                } else if (format.swapBytes == 4) {
                    swappedIndex = (swappedIndex & ~0x3) | (3 - (swappedIndex & 0x3));
                }

                pixelValue |= (address[swappedIndex]) & ((1 << count) - 1);
                remainingBits -= count;
                num -= count;
            };

            ++x;
            address += xIncrement;

            if (x == format.width) {
                x = 0;
                address += yIncrement;
            }

            auto getColorChannel = [](uint32_t pixel,
                                      int bitsPerPixel,
                                      const Qul::Platform::FramebufferFormat::BitField &field) -> uint32_t {
                uint32_t value = (pixel >> (bitsPerPixel - field.offset - field.length)) & ((1 << field.length) - 1);
                if (field.length < 8) {
                    int32_t lowBits = 8 - field.length;
                    value <<= lowBits;

                    // Repeat the set bits, e.g. ABCXXXXX becomes ABCABCAB
                    uint32_t shift = field.length;
                    while (lowBits > 0) {
                        value |= (value >> shift);
                        shift *= 2;
                        lowBits -= shift;
                    }
                }
                return value;
            };

            uint32_t r = getColorChannel(pixelValue, format.bitsPerPixel, format.redChannel);
            uint32_t g = getColorChannel(pixelValue, format.bitsPerPixel, format.greenChannel);
            uint32_t b = getColorChannel(pixelValue, format.bitsPerPixel, format.blueChannel);

            return 0xff000000 | (r << 16) | (g << 8) | b;
        }
    };

    struct ImageData
    {
        PixelData fetchPixel;
        qoi_desc desc;
        uint8_t data[400];
        size_t dataFill;
    };
    ImageData imageData = {{0, xIncrement, yIncrement, address, format},
                           {format.width, format.height, 3, QOI_SRGB},
                           {},
                           0};
    DeviceToHostMessage reply = DeviceToHostMessage_init_zero;
    reply.replyId = callId;
    reply.data.arg = &imageData;
    reply.data.funcs.encode = [](pb_ostream_t *stream, const pb_field_t *field, void *const *arg) -> bool {
        ImageData *imgData = reinterpret_cast<ImageData *>(*arg);
        auto emit = [stream, field, imgData](uint8_t value) {
            imgData->data[imgData->dataFill] = value;
            imgData->dataFill++;
            if (imgData->dataFill >= sizeof(imgData->data)) {
                pb_encode_tag_for_field(stream, field);
                pb_encode_string(stream, &imgData->data[0], imgData->dataFill);
                imgData->dataFill = 0;
            }
        };
        qoi_encode(&imgData->desc, imgData->fetchPixel, emit);
        if (imgData->dataFill > 0) {
            pb_encode_tag_for_field(stream, field);
            pb_encode_string(stream, &imgData->data[0], imgData->dataFill);
        }
        return true;
    };

    reply.which_alternative = DeviceToHostMessage_callDone_tag;
    reply.alternative.callDone.errorCode = ErrorCode_Success;
    reply.alternative.callDone.which_alternative = CallDone_frameBufferDataReply_tag;
    FrameBufferDataReply &fbData = reply.alternative.callDone.alternative.frameBufferDataReply;
    if (layer) {
        fbData.screenId = findScreenId(layer);
        fbData.layerX = layer->x.value();
        fbData.layerY = layer->y.value();
        if (layer->itemExtraProperties.isNull()) {
            fbData.layerZ = 0;
        } else {
            fbData.layerZ = static_cast<uint32_t>(layer->itemExtraProperties->z.value());
        }
    } else {
        fbData.screenId = 0;
        fbData.layerX = 0;
        fbData.layerY = 0;
    }
    sendMessage(&reply);
}

static bool encodeLayers(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    Qul::Private::Items::ItemLayer *itemLayer = (Qul::Private::Items::ItemLayer *) *arg;
    ScreenConfigReply_Layer layer = ScreenConfigReply_Layer_init_zero;
    layer.id = itemLayer->platformId.value();
    return pb_encode_tag_for_field(stream, field) && pb_encode_submessage(stream, &ScreenConfigReply_Layer_msg, &layer);
}

static bool encodeScreens(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    using Qul::Private::Items::Screen;

    // TODO: for now we are limited to a single display using a single layer.
    // For now the core library doesn't allow us to find out anything more.
    Qul::Private::Application *app = (Qul::Private::Application *) *arg;
    auto layer = Qul::Private::Items::ItemLayer::asItemLayer(app->layerForPlatformId(0));

    ScreenConfigReply_Screen screen = ScreenConfigReply_Screen_init_default;
    screen.id = 0;
    screen.width = layer->width.value();
    screen.height = layer->height.value();
    screen.layers.arg = layer;
    screen.layers.funcs.encode = &encodeLayers;
    return pb_encode_tag_for_field(stream, field)
           && pb_encode_submessage(stream, &ScreenConfigReply_Screen_msg, &screen);
}

void handleIncomingMessage(const HostToDeviceMessage *message)
{
    switch (message->which_alternative) {
    case HostToDeviceMessage_hello_tag: {
        struct
        {
            uint32_t major, minor, patch;
        } version = {QUL_DEVICELINK_VERSION_LIST};

        auto app = Qul::Private::Application::instance();
        if (!app) {
            sendError(message->callId, ErrorCode_GenericError, "The application is not started");
            break;
        }

        DeviceToHostMessage reply = DeviceToHostMessage_init_zero;
        reply.replyId = message->callId;
        reply.which_alternative = DeviceToHostMessage_callDone_tag;
        reply.alternative.callDone.errorCode = ErrorCode_Success;
        reply.alternative.callDone.which_alternative = CallDone_helloReply_tag;
        HelloReply &helloReply = reply.alternative.callDone.alternative.helloReply;
        helloReply.qulVersionMajor = version.major;
        helloReply.qulVersionMinor = version.minor;
        helloReply.qulVersionPatch = version.patch;
        helloReply.platform.arg = const_cast<char *>(QUL_PLATFORM_NAME);
        helloReply.platform.funcs.encode = &encodeString;
        sendMessage(&reply);
        break;
    }
    case HostToDeviceMessage_touchEvent_tag: {
        if (!message->alternative.touchEvent.touchPoints_count) {
            sendSuccess(message->callId);
            break;
        }
        if (touchEvent.touchPoints_count) {
            sendError(message->callId, ErrorCode_GenericError, "Touch input still pending");
            break;
        }

        touchEvent = message->alternative.touchEvent;
        touchCallId = message->callId;

        Qul::Private::Application *instance = Qul::Private::Application::instance();
        if (instance) {
            instance->requestEventProcessing();
        }
        break;
    }
    case HostToDeviceMessage_getScreenConfig_tag: {
        DeviceToHostMessage reply = DeviceToHostMessage_init_zero;
        reply.replyId = message->callId;
        reply.which_alternative = DeviceToHostMessage_callDone_tag;
        reply.alternative.callDone.errorCode = ErrorCode_Success;
        reply.alternative.callDone.which_alternative = CallDone_screenConfigReply_tag;
        ScreenConfigReply &screenConfig = reply.alternative.callDone.alternative.screenConfigReply;
        screenConfig.screens.arg = Qul::Private::Application::instance();
        screenConfig.screens.funcs.encode = &encodeScreens;
        sendMessage(&reply);
        break;
    }
    case HostToDeviceMessage_getFramebufferData_tag:
        handleGetFramebufferData(message->callId, message->alternative.getFramebufferData);
        break;
    case HostToDeviceMessage_keyEvent_tag: {
        if (!message->alternative.keyEvent.keyId) {
            sendError(message->callId, ErrorCode_GenericError, "Key input needs a valid key ID");
            break;
        }
        if (keyEvent.keyId) {
            sendError(message->callId, ErrorCode_GenericError, "Another key input call is still running");
            break;
        }

        keyEvent = message->alternative.keyEvent;
        keyCallId = message->callId;

        Qul::Private::Application *instance = Qul::Private::Application::instance();
        if (instance) {
            instance->requestEventProcessing();
        }
        break;
    }
    default:
        sendError(message->callId, ErrorCode_UnsupportedCall, "Call type not supported");
        break;
    }
}

class HDLCIncomingMessage
{
private:
    struct FrameLink
    {
        const DeviceLinkFrame frame;
        size_t frameSize;
        FrameLink *next;

        FrameLink(const DeviceLinkFrame &f, size_t size)
            : frame(f)
            , frameSize(size)
            , next(nullptr)
        {}

        size_t getPayloadSize() { return getPayloadSize(frameSize); }

        static size_t getPayloadSize(size_t frameSize) { return frameSize - sizeof(DeviceLinkFrameHeader); }
    };

    FrameLink *first;
    FrameLink *latest;
    uint16_t frameCount;

    struct FrameReader
    {
        FrameLink *current;
        const DeviceLinkFrame *lastFrame;
        size_t lastFrameSize;
        size_t dataPos;
    };

    static bool readData(pb_istream_t *stream, pb_byte_t *buf, size_t count)
    {
        FrameReader *reader = reinterpret_cast<FrameReader *>(stream->state);
        while (count > 0) {
            const DeviceLinkFrame *frame = reader->current ? &reader->current->frame : reader->lastFrame;
            const size_t frameSize = reader->current ? reader->current->frameSize : reader->lastFrameSize;
            if (!frame) {
                PB_SET_ERROR(stream, "Reading beyond the stream end");
                return false;
            }

            size_t chunkSize = std::min(frameSize - reader->dataPos, count);
            memcpy(buf, &frame->data[reader->dataPos], chunkSize);
            buf += chunkSize;
            count -= chunkSize;
            reader->dataPos += chunkSize;

            if (reader->dataPos >= frameSize) {
                if (reader->current) {
                    reader->current = reader->current->next;
                } else {
                    reader->lastFrame = nullptr;
                }
                reader->dataPos = sizeof(DeviceLinkFrameHeader);
            }
        }
        return true;
    }

    size_t getPayloadSize(size_t finalFrameSize) const
    {
        size_t size = 0;
        for (FrameLink *link = first; link != nullptr; link = link->next) {
            size += link->getPayloadSize();
        }
        return size + FrameLink::getPayloadSize(finalFrameSize);
    }

public:
    HDLCIncomingMessage()
        : first{nullptr}
        , latest{nullptr}
        , frameCount{0}
    {}

    uint16_t getFrameCount() const { return frameCount; }

    /**
     * @brief Resets the incoming message to the initial state (ie. releases all data).
     */
    void reset()
    {
        while (first) {
            FrameLink *next = first->next;
            Qul::PlatformInterface::qul_delete(first);
            first = next;
        }
        latest = first = nullptr;
        frameCount = 0;
    }

    void appendFrame(const DeviceLinkFrame *frame, size_t size)
    {
        if (latest) {
            latest->next = Qul::PlatformInterface::qul_new<FrameLink>(*frame, size);
            latest = latest->next;
            frameCount++;
        } else {
            first = latest = Qul::PlatformInterface::qul_new<FrameLink>(*frame, size);
            frameCount = 1;
        }
    }

    /**
     * @brief Finishes message processing and resets the class to the initial state
     *
     * The final frame is passed here instead of being appended beforehand. This
     * is an optimization: the last frame (which will often be the only frame)
     * does not need an extra dynamic allocation and can be processed directly
     * in the buffer it came in.
     */
    void deliver(const DeviceLinkFrame *finalFrame, size_t finalFrameSize)
    {
        HostToDeviceMessage message = HostToDeviceMessage_init_zero;
        FrameReader reader{first, finalFrame, finalFrameSize, sizeof(DeviceLinkFrameHeader)};

        pb_istream_t stream = {
            &readData,
            &reader,
            getPayloadSize(finalFrameSize),
#ifndef PB_NO_ERRMSG
            nullptr,
#endif
        };
        bool status = pb_decode(&stream, HostToDeviceMessage_fields, &message);
        if (!status) {
            Qul::PlatformInterface::log("Decoding failed: %s\n", PB_GET_ERROR(&stream));
            return;
        }

        hdlcEnabled = true;
        handleIncomingMessage(&message);
        reset();
    }
};

} // namespace

Qul::Platform::DeviceLink *Qul::Platform::DeviceLink::instance()
{
    static Qul::Platform::DeviceLink mInstance;
    return &mInstance;
}

static Qul::PlatformInterface::TouchPoint::State convertTouchPointState(TouchPointState state)
{
    switch (state) {
    case TouchPointState_Pressed:
        return Qul::PlatformInterface::TouchPoint::Pressed;
    case TouchPointState_Moved:
        return Qul::PlatformInterface::TouchPoint::Moved;
    case TouchPointState_Stationary:
        return Qul::PlatformInterface::TouchPoint::Stationary;
    case TouchPointState_Released:
        return Qul::PlatformInterface::TouchPoint::Released;
    default:
        return Qul::PlatformInterface::TouchPoint::Unknown;
    }
}

uint64_t Qul::Platform::DeviceLink::injectInputEvent()
{
    return std::min(injectTouchEvent(), injectKeyEvent());
}

uint64_t Qul::Platform::DeviceLink::injectTouchEvent()
{
    if (!touchEvent.touchPoints_count) {
        return std::numeric_limits<uint64_t>::max();
    }

    if (touchEvent.touchPoints_count > QUL_MAX_TOUCH_POINTS) {
        touchEvent = TouchEvent_init_default;
        sendError(touchCallId, ErrorCode_GenericError, "Operation not supported: too many touch points");
        return std::numeric_limits<uint64_t>::max();
    }

    size_t numPlatformScreens = 0;
    Qul::PlatformInterface::Screen *platformScreens = Qul::Platform::getPlatformInstance()->availableScreens(
        &numPlatformScreens);

    Qul::PlatformInterface::Size virtualScreenSize;
    if (numPlatformScreens > 0)
        virtualScreenSize = Qul::Private::StaticScreenRotationHelpers::rotatedSize(platformScreens[0].size());

    Qul::PlatformInterface::TouchPoint touchPoints[QUL_MAX_TOUCH_POINTS];
    for (uint32_t i = 0; i < touchEvent.touchPoints_count; ++i) {
        touchPoints[i].id = touchEvent.touchPoints[i].id;

        // Qul::PlatformInterface::handleTouchEvent() expects to get touch
        // points in the physical coordinate system of the display, whereas
        // Squish / devicelink uses the virtual coordinate system
        const Qul::PlatformInterface::Point position = Qul::Private::StaticScreenRotationHelpers::mapVirtualToPhysical(
            virtualScreenSize, Qul::PlatformInterface::Point(touchEvent.touchPoints[i].x, touchEvent.touchPoints[i].y));

        touchPoints[i].positionX = position.x();
        touchPoints[i].positionY = position.y();
        touchPoints[i].state = convertTouchPointState(touchEvent.touchPoints[i].state);

        const Qul::PlatformInterface::Size area = Qul::Private::StaticScreenRotationHelpers::rotatedSize(
            Qul::PlatformInterface::Size(static_cast<PlatformInterface::coord_t>(touchEvent.touchPoints[i].areaX),
                                         static_cast<PlatformInterface::coord_t>(touchEvent.touchPoints[i].areaY)));

        if (touchEvent.touchPoints[i].has_areaX) {
            touchPoints[i].areaX = area.width();
        }
        if (touchEvent.touchPoints[i].has_areaY) {
            touchPoints[i].areaY = area.height();
        }
        if (touchEvent.touchPoints[i].has_pressure) {
            touchPoints[i].pressure = touchEvent.touchPoints[i].pressure;
        }
        if (touchEvent.touchPoints[i].has_rotation) {
            touchPoints[i].rotation = touchEvent.touchPoints[i].rotation;
        }
    }

    const auto timestamp = Qul::Platform::getPlatformInstance()->currentTimestamp();
    Qul::PlatformInterface::handleTouchEvent(NULL, timestamp, &touchPoints[0], touchEvent.touchPoints_count);
    touchEvent = TouchEvent_init_default;
    sendSuccess(touchCallId);
    return std::numeric_limits<uint64_t>::max();
}

uint64_t Qul::Platform::DeviceLink::injectKeyEvent()
{
    using namespace Qul::PlatformInterface;
    if (!keyEvent.keyId) {
        return std::numeric_limits<uint64_t>::max();
    }

    // QUL currently doesn't handle the modifiers, UTF-8 text and the autoRepeat flag
    // In case it is ever supported we will add optional fields to the KeyEvent message
    const auto timestamp = Qul::Platform::getPlatformInstance()->currentTimestamp();
    handleKeyEvent(timestamp,
                   (keyEvent.type == KeyEventType_KeyPressed) ? KeyPressEvent : KeyReleaseEvent,
                   keyEvent.keyId,
                   keyEvent.nativeScanCode,
                   NoKeyboardModifier,
                   NULL,
                   false,
                   keyEvent.codePoint);
    keyEvent = KeyEvent_init_default;
    sendSuccess(keyCallId);
    return std::numeric_limits<uint64_t>::max();
}

void Qul::Platform::DeviceLink::frame_handler(const uint8_t *frame_buffer, uint16_t frame_length)
{
    if (frame_length < sizeof(DeviceLinkFrameHeader)) {
        return;
    }

    static HDLCIncomingMessage incomingMessage;

    const DeviceLinkFrame *frame = reinterpret_cast<const DeviceLinkFrame *>(frame_buffer);
    uint16_t index = frame->header.getIndex();

    // Out-of-order message means a protocol error, we skip until the next zero frame
    if (index != incomingMessage.getFrameCount()) {
        incomingMessage.reset();
        if (index != 0) {
            return;
        }
    }

    if (frame->header.status) {
        incomingMessage.deliver(frame, frame_length);
    } else {
        incomingMessage.appendFrame(frame, frame_length);
    }
}

namespace {
class DeviceLinkReceiveQueue
    : public Qul::EventQueue<uint8_t, Qul::EventQueueOverrunPolicy_Discard, 100> // Up to: MINIHDLC_MAX_FRAME_LENGTH
{
    void onEvent(const uint8_t &c) override { minihdlc_char_receiver(c); }
    void onQueueOverrun() override
    {
        clearOverrun();
        Qul::PlatformInterface::log("Device link receiver queue overrun. Consider increasing the queue size.\n");
    }
};

DeviceLinkReceiveQueue &deviceLinkReceiveQueue()
{
    static DeviceLinkReceiveQueue instance;
    return instance;
}
} // namespace

void Qul::Platform::DeviceLink::init()
{
    deviceLinkReceiveQueue(); // This initializes the static variable. It should not be initialized when called from an interrupt.
    Qul::Private::setTouchInjectionCallback(
        []() -> uint64_t { return Qul::Platform::DeviceLink::instance()->injectInputEvent(); });
    minihdlc_init(&SendCharWrapper, &frame_handler);
    Qul::Platform::getDeviceLinkInterface()->platformInit();
}

// This function is called from an interrupt
void Qul::Platform::DeviceLink::charsReceived(const uint8_t *data, int size)
{
    while (size--)
        deviceLinkReceiveQueue().postEventFromInterrupt(*data++);
    Qul::Private::Application *instance = Qul::Private::Application::instance();
    if (instance) {
        instance->requestEventProcessing();
    }
}

// Buffer for printing messages on terminal
static char
    messageBuffer[80]; // Maximum message size. If changed, do it in deviceinformationexchangeprotocol.options as well.
static unsigned int messageSize = 0;
static char *messagePointer = messageBuffer;

void Qul::Platform::DeviceLink::printMessage(char *message, int length)
{
    for (int i = 0; i < length; i++) {
        *messagePointer = *message;
        messageSize++;

        // There need to be size for \r and \0 left in the message
        if (*message == '\n' || messageSize >= sizeof(messageBuffer) - 2) {
            if (*message == '\n') {
                // Add additional carriage return
                messagePointer++;
                *messagePointer = '\r';
                messageSize++;
            }

            if (hdlcEnabled) {
                // Zero terminate the message
                messagePointer++;
                *messagePointer = '\0';
                messageSize++;

                DeviceToHostMessage messageEnvelope = DeviceToHostMessage_init_zero;
                // Maybe use variable length callbacks as alternative
                messageEnvelope.which_alternative = DeviceToHostMessage_consoleLog_tag;
                memcpy(&messageEnvelope.alternative.consoleLog.data, messageBuffer, messageSize);
                sendMessage(&messageEnvelope);
            } else {
                Qul::Platform::getDeviceLinkInterface()->transmitChars((uint8_t *) messageBuffer, messageSize);
            }
            messagePointer = messageBuffer;
            messageSize = 0;
        } else {
            messagePointer++;
        }

        message++;
    }
}

#else  // QUL_PLATFORM_DEVICELINK_ENABLED

Qul::Platform::DeviceLink *Qul::Platform::DeviceLink::instance()
{
    return NULL;
}

uint64_t Qul::Platform::DeviceLink::injectInputEvent()
{
    QUL_ASSERT(false, QulError_DeviceLink_ShouldNotBeCalled);
    return std::numeric_limits<uint64_t>::max();
}

void Qul::Platform::DeviceLink::init()
{
    QUL_ASSERT(false, QulError_DeviceLink_ShouldNotBeCalled);
    return;
}

void Qul::Platform::DeviceLink::charsReceived(const uint8_t *data, int size)
{
    QUL_UNUSED(data);
    QUL_UNUSED(size);
    QUL_ASSERT(false, QulError_DeviceLink_ShouldNotBeCalled);
    return;
}

void Qul::Platform::DeviceLink::printMessage(char *message, int length)
{
    QUL_UNUSED(message);
    QUL_UNUSED(length);
    QUL_ASSERT(false, QulError_DeviceLink_ShouldNotBeCalled);
    return;
}
#endif // QUL_PLATFORM_DEVICELINK_ENABLED

/*!
    \ingroup platformapi
    \headerfile <platform/devicelink.h>

    \title Platform device link functionality

    \brief Provides device link functionality for the platform.

    The device link component enables communication between host and device. It
    allows them to exchange touch, screenshot, performance, or log data, for
    debugging or testing purposes.

    \sa{Porting DeviceLink Communication}

    \section1 Classes or structs
    \annotatedlist header-devicelink
*/

/*!
    \class Qul::Platform::DeviceLinkInterface
    \relatedheader devicelink
    \inheaderfile platform/devicelink.h
    \since \QULPlatform 2.3

    \brief This provides the interface to implement by the platform.

    Each platform using the device link has to provide an implementation of this interface.

    \sa Qul::Platform::DeviceLink
    \sa{Porting DeviceLink Communication}
*/

/*!
    \fn void Qul::Platform::DeviceLinkInterface::transmitChars(const uint8_t *data, uint32_t size)

    \brief Send a data stream to the host after it has been packed and framed.

    This function is called when there is message data to be transferred to the host.
    The implementation has to use the serial port write functions to send the \a data with \a size.
*/

/*!
    \fn void Qul::Platform::DeviceLinkInterface::platformInit()

    \brief Perform last minute platform specific setup.

    This function is called after the DeviceLink component is ready to send and receive data.
    The implementation may enable the serial port interrupts or set other flags that
    enable sending and receiving actual data.
*/

/*!
    \fn Qul::Platform::FramebufferFormat Qul::Platform::DeviceLinkInterface::framebufferFormat(const PlatformInterface::LayerEngine::ItemLayer *layer)

    \brief Get information about a framebuffer for the given \a layer.

    This is used by the device link implementation to read the data and transfer
    it to the host when a screenshot is requested.

    \note The \a layer parameter can be ignored if the platform does not support
    multiple layers.
*/

/*!
    \fn uint64_t Qul::Platform::DeviceLink::injectInputEvent()
    \brief Injects an input event after an input event message has been received.

    When a message with an input event has been received this function is called
    to inject a single event from the buffer to the event loop and remove it from the queue.
    This function has to be called from the main loop at the given time being returned from
    nextTouchTimestamp.

    After an input sequence message has been received, this function returns
    the timestamp at which the next input event has to be injected by calling
    \l injectInputEvent.
    When there are no more input events in the current sequence this function
    returns the maximum integer value.
*/

/*!
    \fn void Qul::Platform::DeviceLink::printMessage(char *message, int length)
    \brief Sends a logging message to the host

    The logging facilities of \QUL are using this function to send a log message to the host.
    If the transfer protocol has been enabled it will be wrapped in the protocol and framing.
    If the transfer protocol has not been enabled yet, it will be sent over the serial port as is.
    \a message and \a length specify the string to be sent.
*/

/*!
    \class Qul::Platform::DeviceLink
    \relatedheader devicelink
    \inheaderfile platform/devicelink.h
    \since \QULPlatform 2.3

    \brief This class provides functionality for communication between host and device.

    Methods of this class provide the ability for communition between the host machine and the device
    by sending messages over the serial port of a device.
    It uses ProtoBuf and HDLC for sending and framing the messages.

    The protocol and framing will be disabled until the first byte from the host is received.
    This allows to see any log output in the terminal without having to use special tools.
    Once the first byte from the host is received, the protocol and framing is turned on so that
    in order to read debugging messages, there are tools required to decode them.

    \sa Qul::Platform::DeviceLinkInterface
*/

/*!
    \fn void Qul::Platform::DeviceLink::charsReceived(const uint8_t *data, int size)
    \brief Decodes and processes data received on the serial port.

    This function will be called from \l Qul::PlatformInterface::deviceLinkBytesReceived and handles the data
    specified by \a data and \a size.

    \sa{Porting DeviceLink Communication}
*/

/*!
    \fn void Qul::Platform::DeviceLink::init()
    \brief Initializes the device link capability.

    This function has to be called after the serial communication hardware has been initialized and is
    ready to be used.
*/

/*!
    \fn Qul::Platform::getDeviceLinkInterface *Qul::Platform::getDeviceLinkInterface()

    \brief Provides the device link interface implementation for the platform.

    If there is no such imlementation the function can return \c nullptr. But ideally, when the device link
    is not compiled in, this function should not be called at all.

    \sa QUL_PLATFORM_DEVICELINK_ENABLED
*/

/*!
    \ingroup platformapi
    \headerfile <platform/framebufferformat.h>

    \title Framebuffer format

    \brief Provides information about framebuffer format for use with the device
    link protocol.

    \sa{Porting DeviceLink Communication}

    \section1 Classes or structs
    \annotatedlist header-framebufferformat
*/

/*!
    \class Qul::Platform::FramebufferFormat
    \relatedheader framebufferformat
    \inheaderfile platform/framebufferformat.h
    \since \QULPlatform 2.3

    \brief This struct contains information about a framebuffer.

    The data in this struct is used to interpret the contents of a framebuffer.

    \sa Qul::Platform::DeviceLinkInterface::framebufferFormat()
    \sa{Porting DeviceLink Communication}
*/

/*!
    \variable Qul::Platform::FramebufferFormat::address

    \brief The address of the framebuffer.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::width

    \brief The width in pixels of the framebuffer.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::height

    \brief The height in pixels of the framebuffer.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::bitsPerPixel

    \brief The bits per pixel of the framebuffer.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::bytesPerLine

    \brief The bytes per line (stride) of the framebuffer.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::redChannel

    \brief Offset and length of the bits in the red color channel.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::greenChannel

    \brief Offset and length of the bits in the green color channel.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::blueChannel

    \brief Offset and length of the bits in the blue color channel.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::swapBytes

    \brief Group size of bytes to swap.

    If set to 0, no swapping is done. If set to 2, every pair of bytes are
    swapped. If set to 4, every set of four bytes are reversed, so that BGRA
    becomes ARGB.
*/

/*!
    \class Qul::Platform::FramebufferFormat::BitField

    \brief This struct contains information about the bits of a color channel in a framebuffer.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::BitField::offset

    \brief The offset of the bits.
*/

/*!
    \variable Qul::Platform::FramebufferFormat::BitField::length

    \brief The number of bits.
*/

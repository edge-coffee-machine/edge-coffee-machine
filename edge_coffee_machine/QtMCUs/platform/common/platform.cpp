// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QUL_PLATFORM_SKIP_PLATFORM_CONFIG
#include "platform_config.h"
#endif

#include <qul/global.h>
#include <platform/platform.h>
#include <platforminterface/platforminterface.h>
#include <platforminterface/screen.h>

namespace Qul {
namespace Platform {

/*!
    \struct Qul::Platform::Config
    \relatedheader platform
    \inheaderfile platform/platform.h
    \since \QULPlatform 2.0

    \brief This struct provides runtime representation of platform configuration.

    Fields of this struct reflects build-time platform properties:
    \l MCU.Config.platformImageAlignment
    \l MCU.Config.platformPixelWidthAlignment
    \l QUL_PLATFORM_DEFAULT_TEXT_CACHE_SIZE
    \l QUL_PLATFORM_DEFAULT_NUM_FRAMES_TO_PRESERVE_ASSETS
    \l QUL_PLATFORM_DEFAULT_TEXT_CACHE_ENABLED
    \l QUL_PLATFORM_RENDER_BATCH_HEIGHT and
    \l QUL_PLATFORM_DEFAULT_LAYER_RENDERING_HINTS.

    \sa {Defining default variables for the platform}
*/

Config qulPlatformConfig()
{
    Config config = {QUL_PLATFORM_REQUIRED_IMAGE_ALIGNMENT,
                     QUL_PLATFORM_REQUIRED_PIXEL_WIDTH_ALIGNMENT,
                     QUL_PLATFORM_DEFAULT_TEXT_CACHE_SIZE,
                     QUL_PLATFORM_DEFAULT_NUM_FRAMES_TO_PRESERVE_ASSETS,
                     QUL_PLATFORM_DEFAULT_TEXT_CACHE_ENABLED ? true : false,
                     QUL_PLATFORM_RENDER_BATCH_HEIGHT,
                     QUL_PLATFORM_DEFAULT_LAYER_RENDERING_HINTS};
    return config;
}

Config PlatformContext::config() const
{
    return qulPlatformConfig();
}

/*!
    \namespace Qul::Platform
    \brief The namespace offers interfaces to port \QUL to new platforms.
*/

/*!
    \ingroup platformapi
    \headerfile <platform/platform.h>

    \title Platform abstraction functionality

    \brief Provides abstraction functionality for the platform.

    This namespace contains all functions of the platform abstraction that are used
    by the \QUL core library. The header files for this namespace are
    located in \c{include/platform/}

    \section1 Classes or structs
    \annotatedlist header-platform
*/

/*!
    \class Qul::Platform::PlatformContext
    \relatedheader platform
    \inheaderfile platform/platform.h
    \since \QULPlatform 2.0

    \brief This class provides an abstract interface to implement platform context.

    Methods of this class are platform APIs for hardware initialisation, execution loop and platform
    level graphics implementation. When porting to a new platform, a class derived from this class has to be implemented
    and the virtual methods are being overridden to get platform specific implementation. \QUL core use an instance
    of this class to communicate with the hardware. See \l{\QUL Platform Porting Guide} for more information.
*/

/*!
    \enum Qul::Platform::FrameBufferingType
    \relates <platform/platform.h>
    \since  \QULPlatform 1.5

    Defines the different buffering mechanisms that are supported.

    \value SingleBuffering
           There is only single buffer used for rendering. All rendering must happen within timing
           budget allowed by the display refresh rate.
    \value FlippedDoubleBuffering
           Two buffers are used and both can be displayed. One is used for displaying while the
           other is used for rendering and then flipped.
    \value CopyingDoubleBuffering
           There is one buffer for rendering and one for displaying. When rendering is finished the
           content from rendering buffer is copied to the display buffer.
    \value PartialBuffering
           There is one or multiple buffers which are smaller than full framebuffer. Partial buffering
           requires that display has built-in memory.
    \value OtherBuffering
           Other platform specific buffering type.

    \sa {Framebuffer Requirements}, {Partial framebuffer}
*/

/*!
    \struct Qul::Platform::FrameStatistics
    \relatedheader platform
    \inheaderfile  platform/platform.h
    \since  \QULPlatform 1.5

    \brief Provides frame rendering statistics.

    FrameStatistics provides information about possible delays in rendering and the timing budget
    to render frames.

    \sa Qul::Platform::PlatformContext::presentFrame()
*/

/*!
    \fn Qul::PlatformContext *getPlatformInstance()
    \relates  <platform/platform.h>
    \since \QULPlatform 2.0

    \brief Returns hardware specific platform context instance.
*/

/*!
    \fn Qul::Platform::FrameStatistics::FrameStatistics()
    \since  \QULPlatform 1.5

    \brief Constructs default FrameStatistics object.

    Default constructor that initializes \l refreshDelta and \l remainingBudget with the default
    values, 0 and the maximum integer value for the platform respectively.
*/

/*!
    \variable Qul::Platform::FrameStatistics::refreshDelta
    \since  \QULPlatform 1.5

    \brief Frame delay relative to its refresh interval.

    This variable indicates how much the last frame was delayed relative to its refresh interval.
    When refresh interval is greater than 1, using a negative \c refreshDelta would reduce the
    refresh interval to avoid skipping frames.

    \sa Qul::Platform::PlatformContext::beginFrame(), Qul::Platform::PlatformContext::presentFrame()
*/

/*!
    \variable Qul::Platform::FrameStatistics::remainingBudget
    \since  \QULPlatform 1.5

    \brief Remaining timing budget for rendering.

    This variable indicates how many milliseconds are spared before skipping a frame. It can be
    used to preemptively increase the refresh interval when the budget is very tight, to avoid
    skipping any frames.

    \sa Qul::Platform::PlatformContext::beginFrame(), Qul::Platform::PlatformContext::presentFrame()
*/

/*!
    \struct Qul::Platform::PerformanceMetrics
    \relatedheader platform
    \inheaderfile  platform/performancemetrics.h
    \since  \QULPlatform 2.6

    \brief Provides performance metrics.

    PerformanceMetrics provides functions to access platform-provided performance data.
*/

/*!
    \fn Qul::Platform::PerformanceMetrics::maxHeapUsage()
    \since  \QULPlatform 2.6

    \brief Returns the maximum number of bytes allocated from the heap memory.

    \note Default implementation returns 0.
*/

/*!
    \fn Qul::Platform::PerformanceMetrics::maxStackUsage()
    \since  \QULPlatform 2.6

    \brief Returns the maximum number of bytes allocated from the stack.

    \note Default implementation returns 0.
*/

/*!
    \fn Qul::Platform::PerformanceMetrics::cpuLoad()
    \since  \QULPlatform 2.6

    \brief Returns the current CPU load. The value is the percentage of active
    execution time between two calls to this function. The first call returns the CPU load
    since startup.

    \note Default implementation returns 0.
*/

/*!
    \fn Qul::Platform::PlatformContext::scheduleEngineUpdate(uint64_t timestamp)

    Requests the platform implementation to schedule a \QUL engine update
    at \a timestamp.

    This function is supposed to be only called from the \QUL engine during
    execution of updateEngine().
    It requests the platform implementation to schedule an update of the
    \QUL engine at the given timestamp.
    The timestamp is calculated based on the \QUL engine timers and
    running animations.

    A timestamp lesser than the current timestamp, or even 0, should result in calling
    updateEngine as soon as possible. A greater timestamp value than the current
    timestamp means that the platform implementation should call updateEngine at
    that given time. Until the scheduled time, the device may enter a sleep mode.
    \l Qul::PlatformInterface::updateEngine should not be called earlier than required.

    For an example usage see the exec() or updateEngine() documentation.

    \note The implementation of this function can get called from an interrupt and it must be safe
    for running within an interrupt context.

    \sa Qul::PlatformInterface::updateEngine(), Qul::Platform::PlatformContext::exec()
 */

/*!
    \fn Qul::Platform::PlatformContext::initializeHardware()

    \brief Initializes hardware required by \QUL.

    This may include clocks, pins, peripherals, buses and memories.

    Developer may also decide not to implement it and perform own hardware initialization.

    \note On static library configuration this function won't get called if \c{qul_run()} is
    called with \c{initializeHardware} set to \c{false}.

    Called by \l{Qul::initHardware()}.
*/

/*!
    \fn Qul::Platform::PlatformContext::initializePlatform()

    \brief Initializes \QUL platform specific components.

    This may include resource preloading and initializing CPU-based fallback drawing
    engine in \QUL core.

    Called by \l{Qul::initPlatform()}.
*/

/*!
    \fn Qul::Platform::PlatformContext::initializeDisplay(const Qul::PlatformInterface::Screen *screen)

    \brief Initializes the platform display.

    Initializes the platform display, with the screen geometry provided by the
    \a screen parameter. Called some time after \l initializeHardware.

    \note Default implementation does nothing.
*/

/*!
    \fn Qul::Platform::PlatformContext::exec()

    \brief Runs the main exec loop provided by the platform.

    The exec loop will run forever, or at least for as long as the application
    is running, and is responsible for calling
    Qul::PlatformInterface::updateEngine at appropriate times. When no updates
    need to performed, it should yield or enter a sleep state if possible.

    Here's an example showing how the exec() function might look:

    \snippet example-baremetal/platform_context.cpp nextUpdate
    \dots 0
    \snippet example-baremetal/platform_context.cpp exec

    \note The function body can be left empty if you are not going to use any demos, examples, tests, calling Application::exec() or \l{Using app_common}{\c{app_common}} framework.

    \sa Qul::PlatformInterface::updateEngine()
*/

/*!
    \fn Qul::Platform::PlatformContext::update()

    \brief Performs a single update of the \QUL core engine.

    This function performs all actions to update the \QUL core engine once.
    It calls \l Qul::PlatformInterface::updateEngine if the time is due.

    Here's an example showing how the update() function might look:

    \snippet example-baremetal/platform_context.cpp update

    \sa Qul::PlatformInterface::updateEngine, Qul::Platform::PlatformContext::exec
*/

/*!
    \fn Qul::Platform::FrameBufferingType Qul::Platform::PlatformContext::frameBufferingType(const PlatformInterface::LayerEngine::ItemLayer* layer) const

    \brief Returns layer's buffering type.

    Returns FrameBufferingType for the given \a layer.

    \note Default implementation returns FrameBufferingType::SingleBuffering.

    \sa FrameBufferingType
*/

/*!
    \fn Qul::PlatformInterface::LayerEngine *Qul::Platform::PlatformContext::layerEngine()

    \brief Returns the layer engine for the platform.

    Returns the layer engine responsible for allocating, updating and
    deallocating hardware layers. If the platform doesn't support multiple
    layers, nullptr can be returned.

    \note Default implementation returns \c nullptr .
*/

/*!
    \fn Qul::PlatformInterface::DrawingDevice *Qul::Platform::PlatformContext::beginFrame(const Qul::PlatformInterface::LayerEngine::ItemLayer *layer, const Qul::PlatformInterface::Rect &rect, int refreshInterval)

    \brief Begins \QUL frame rendering.

    Marks the beginning of \QUL frame rendering for a given \a layer, and
    returns a DrawingDevice for it. The \a rect value specifies the dirty area
    that will be drawn to. The \a refreshInterval value specifies when to show
    the new frame, relative to when the last frame was shown. A refreshInterval
    of -1 indicates that the new frame should be shown immediately.

    \note Default implementation does nothing.

    \sa endFrame(), presentFrame(), Qul::PlatformInterface::DrawingDevice, Qul::PlatformInterface::Rect
*/

/*!
    \fn void Qul::Platform::PlatformContext::endFrame(const Qul::PlatformInterface::LayerEngine::ItemLayer *layer)

    \brief Ends \QUL frame rendering.

    Marks the end of \QUL frame rendering for the given \a layer.

    \note Default implementation does nothing.

    \sa beginFrame(), presentFrame()
*/

/*!
    \fn Qul::Platform::FrameStatistics Qul::Platform::PlatformContext::presentFrame(const Qul::PlatformInterface::Screen *screen, const Qul::PlatformInterface::Rect &rect)

    \brief Presents layer and frame updates to the screen and returns statistics.

    \QUL core calls \c presentFrame for a given \a screen after all its layer
    updates have been done using the \l Qul::Platform::PlatformContext::layerEngine, and all
    updated frame contents have been rendered using \l beginFrame and \l
    endFrame.

    The \a rect parameter specifies the bounding rectangle of the areas of the
    screen that have changed relative to the last frame, assuming no layer
    positions have changed.

    Platforms that support it should update the background color based on \l
    Qul::PlatformInterface::Screen::backgroundColor().

    \note \l presentFrame might get called without \l beginFrame or endFrame
    being called first, if layer properties such as position or opacity have
    changed, or if the screen's background color has changed. Default implementation
    does nothing.

    \sa beginFrame(), endFrame(), Qul::Platform::FrameStatistics, Qul::PlatformInterface::Rect, Qul::PlatformInterface::Screen
*/

/*!
    \fn Qul::Platform::PlatformContext::waitUntilAsyncReadFinished(const void *begin, const void *end)

    \brief Block until no asynchronous transfer is reading from the given memory range.

    Some platforms support asynchronous transfers, like DMA2D (STM) or PXP
    (NXP), that are used for drawing and blending. If the CPU writes to memory
    that is currently being read from by some asynchronous blending operation,
    then that could cause unwanted graphical glitches.

    Use this function to wait until all asynchronous reads from the memory area
    specified by \a begin and \a end have finished. The range is [\a begin, \a
    end), meaning \a end is not included but serves to indicate the end of the
    range.

    \note Default implementation does nothing.
*/

/*!
    \fn Qul::Platform::PlatformContext::flushCachesForAsyncRead(const void *address, size_t length)

    \brief This function is called to synchronize a given area of memory before an asynchronous read.

    For asynchronous transfers, such as blitting and blending using DMA2D (STM) or
    PXP (NXP), it might be necessary to invalidate some caches (like D-Cache on
    ARM based CPUs) so that any changes to memory are actually fully committed
    before the transfer begins.

    Use this function to indicate that the memory range specified by \a address
    and \a length might have been modified, and that caches need to be flushed to
    ensure that asynchronous transfers read the correct up-to-date data.

    \note Default implementation does nothing.
*/

/*!
    \fn double Qul::Platform::PlatformContext::rand()

    \brief Return a uniformly random value in the range [0, 1).

    This function returns a random value in the range [0, 1), meaning 1 should
    not be inclusive. It might use a hardware random number generator if
    available. Otherwise, the C++11 std::minstd_rand or a custom software RNG
    can be used.
*/

/*!
    \fn uint64_t Qul::Platform::PlatformContext::currentTimestamp()

    \brief Returns the currently elapsed system time, in milliseconds.
*/

/*!
    \fn std::pair<void *, void *> Qul::Platform::stackRange()
    \internal
    \relates <platform/private/mem_info.h>
    \since \QUL 1.0

    \brief Returns the stack memory address range of the platform.

    This function returns the memory range that contains the platform stack. The
    first element of the returned pair is the top of the stack, and the second
    element is the stack limit (which can be a smaller address than the top of
    the stack).

    The \QUL platform library uses this information to provide profiling information
    about how much stack space is consumed by an application.
*/

/*!
    \fn Qul::PlatformInterface::Screen *Qul::Platform::PlatformContext::availableScreens(size_t* screenCount) const

    \brief Returns array of the available screens. If there is no screen available function returns \c nullptr. Number of the items in the array is returned in \a screenCount output pointer.

    The screen objects returned provide information about the screen dimensions
    as well as a swap chain used to draw to and present buffers for a particular display.
    First item in the returned array is treated as a default, primary screen.

    \note The memory allocated for the return value is owned by the platform. There is no transfer of ownership. Default implementation returns \c nullptr .
*/

/*!
    \fn Qul::PlatformInterface::MemoryAllocator* PlatformContext::memoryAllocator(Qul::PlatformInterface::MemoryAllocator::AllocationType type)

    \brief Provides a memory allocator

    Returns a Qul::PlatformInterface::MemoryAllocator pointer to handle memory
    allocations of the given \a type.

    \note The returned pointer is owned by the platform and should not be freed. Default implementation returns pointer to static instance of Qul::PlatformInterface::MemoryAllocator .

    \sa Qul::PlatformInterface::MemoryAllocator
*/

/*!
    \fn void Qul::Platform::PlatformContext::consoleWrite(char character)

    \brief Writes one \a character to the console.

    Used by the \QUL core library to print log data.

    \note Default implementation does nothing.

    \sa Qul::PlatformInterface::log()
    \sa qul_printf()
*/

/*!
    \fn size_t Qul::Platform::PlatformContext::partialBufferPixelCount(const Qul::PlatformInterface::LayerEngine::ItemLayer *layer) const

    \brief Returns the pixel count of a single partial framebuffer for a given \a layer.

    The \QUL platform library uses this information to limit the size of the rect in \l beginFrame when partial buffering is used.

    Minimum pixel count is the width of the layer. Bigger pixel count will reduce rendering overhead.

    \note Default implementation returns 0.

    \sa Qul::Platform::PlatformContext::beginFrame()
    \sa PartialBuffering
*/

/*!
    \fn PlatformInterface::Rect Qul::Platform::PlatformContext::adjustedPartialUpdateRect(const PlatformInterface::LayerEngine::ItemLayer *layer, const PlatformInterface::Rect &rect) const

    \brief Returns a platform-specific adjusted rect for a given \a layer when partial buffering is used.

    The \QUL core library uses this information to adjust the size of the dirty area when partial buffering is used.

    The adjusted rect will later be passed to \l beginFrame() to draw the area corresponding to the partial update.

    Some platforms might have certain alignment requirements or a minimum size for the update rectangle.

    \note Default implementation returns \a rect.

    \sa Qul::Platform::PlatformContext::beginFrame()
    \sa PartialBuffering
*/

/*!
    \fn Qul::Platform::PerformanceMetrics Qul::Platform::PlatformContext::performanceMetrics()
    \since  \QULPlatform 2.6

    \brief Provides an instance of \l Qul::Platform::PerformanceMetrics

    Its main purpose is to provide access to platform performance measurement data for
    the \QUL core library. This data is exposed to user applications via the \l QulPerf
    QML type.
*/

/*!
    \fn Qul::Platform::Config Qul::Platform::PlatformContext::config() const

    \brief Provides instance of Qul::Platform::Config.

    Its main purpose is to provide platform configuration to the \QUL core library.

    \note Default implementation is delivered with \e platform/common/platform.cpp,
        which must be linked with the custom platform implementation.
*/

} // namespace Platform
} // namespace Qul

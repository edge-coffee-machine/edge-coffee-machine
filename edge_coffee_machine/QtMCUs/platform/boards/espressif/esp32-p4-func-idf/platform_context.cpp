// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include "platform_display.h"
#include "platform_os.h"
#include "platform_touch.h"
#include "platform_drawing.h"

#include <platforminterface/drawingdevice.h>
#include <platforminterface/drawingengine.h>
#include <platforminterface/memoryallocator.h>
#include <platforminterface/rect.h>
#include <platforminterface/screen.h>
#include <platform/platform.h>
#include <platform/devicelink.h>
#include <qul/pixelformat.h>
#include <esp_log.h>

#include <esp_timer.h>
#include <bsp/display.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rom/ets_sys.h>

#include <random>

#include "ppadrawingengine.h"

#include "driver/i2c_master.h"
#include "bsp/esp-bsp.h"

namespace Qul {
namespace Platform {
namespace Private {

uint64_t nextUpdate = 0;
static int refreshInterval = -1;

PlatformInterface::Rect flushRect;
void swapBuffers();
} // namespace Private

struct Esp32Platform : PlatformContext
{
    void initializeHardware(void)
    {
        bsp_i2c_init();
        i2c_master_bus_handle_t i2c_bus = bsp_i2c_get_handle();
        
        if (Qul::Platform::DeviceLink::instance()) {
            Qul::Platform::DeviceLink::instance()->init();
        }
    }

    void initializePlatform() override
    {
        initSuspension();
#ifdef QUL_PLATFORM_ENABLE_PPA_DRAWING_ENGINE
        Private::PpaDrawingEngine::init();
#endif
#if QUL_COLOR_DEPTH == 16
        Qul::PlatformInterface::init16bppRendering();
#elif QUL_COLOR_DEPTH == 24
        Qul::PlatformInterface::init24bppRendering();
#else
#error "Unsupported color depth. Supported values are 16 and 24."
#endif
    }

    void initializeDisplay(const PlatformInterface::Screen *) override
    {
        Private::initializeLcd();
        Private::touchInit();
    }

    void exec() override
    {
        while (true) {
            Private::touchRead();

            const uint64_t nextUpdate = this->update();
            const uint64_t now = currentTimestamp();

            if (nextUpdate > now)
                suspend(MAINLOOP_SEMAPHORE, nextUpdate - now);
        }
    }

    uint64_t update() override
    {
        const uint64_t timestamp = currentTimestamp();

        if (timestamp >= Private::nextUpdate) {
            Qul::PlatformInterface::updateEngine(timestamp);
        }

        return Private::nextUpdate;
    }

    double rand() override
    {
        return m_randomEngine() / (m_randomEngine.max() + 1.0);
    }

    void scheduleEngineUpdate(uint64_t timeout) override
    {
        Private::nextUpdate = timeout;

        if (currentTimestamp() >= Private::nextUpdate)
            resume(MAINLOOP_SEMAPHORE);
    }

    uint64_t currentTimestamp() override
    {
        return esp_timer_get_time() / 1000;
    }

    FrameBufferingType frameBufferingType(const PlatformInterface::LayerEngine::ItemLayer *layer) const override
    {
        QUL_UNUSED(layer);
        return FlippedDoubleBuffering;
    }

    size_t partialBufferPixelCount(const PlatformInterface::LayerEngine::ItemLayer *layer) const override
    {
        QUL_UNUSED(layer);
        return Private::lcdFrameBufferPixelCount();
    }

    Qul::PlatformInterface::Screen *availableScreens(size_t *screenCount) const override
    {
        *screenCount = 1;

        static PlatformInterface::Screen screen(PlatformInterface::Size(Private::lcdWidth(), Private::lcdHeight()),
                                                Private::lcdPixelFormat());
        return &screen;
    }

    PlatformInterface::DrawingDevice *beginFrame(const PlatformInterface::LayerEngine::ItemLayer *layer,
                                                 const PlatformInterface::Rect &rect,
                                                 int refreshInterval) override
    {
        QUL_UNUSED(layer);
        QUL_UNUSED(rect);

        Private::refreshInterval = refreshInterval;

        uchar *bits = Private::backBuffer();

#ifdef QUL_PLATFORM_ENABLE_PPA_DRAWING_ENGINE
        static Private::PpaDrawingEngine drawingEngine;
#else
        static PlatformInterface::DrawingEngine drawingEngine;
#endif

        static PlatformInterface::DrawingDevice buffer = {Private::lcdPixelFormat(),
                                                          PlatformInterface::Size(Private::lcdWidth(),
                                                                                  Private::lcdHeight()),
                                                          bits,
                                                          Private::lcdWidth() * Private::lcdBytesPerPixel(),
                                                          &drawingEngine,
                                                          nullptr};
        buffer.setBits(bits);

#ifdef QUL_PLATFORM_ENABLE_PPA_DRAWING_ENGINE
        Private::PpaDrawingEngine::beginFrame(bits,
                                              Private::lcdWidth(),
                                              Private::lcdHeight(),
                                              Private::lcdWidth() * Private::lcdBytesPerPixel(),
                                              Private::lcdPixelFormat());
#endif
        return &buffer;
    }

    void endFrame(const PlatformInterface::LayerEngine::ItemLayer *layer) override
    {
        QUL_UNUSED(layer);
    }

    FrameStatistics presentFrame(const PlatformInterface::Screen * /*screen*/,
                                 const PlatformInterface::Rect &rect) override
    {
#ifdef QUL_PLATFORM_ENABLE_PPA_DRAWING_ENGINE
        Private::PpaDrawingEngine::flush();
#endif
        Private::swapBuffers();
        PlatformInterface::Rect fullScreen(0, 0, Private::lcdWidth(), Private::lcdHeight());
        Private::flushLcd(fullScreen, Private::frontBuffer());
        return FrameStatistics();
    }

    PlatformInterface::MemoryAllocator *memoryAllocator(PlatformInterface::MemoryAllocator::AllocationType type)
    {
        return PlatformContext::memoryAllocator(type);
    }

    void consoleWrite(char character) override
    {
        static char logBuffer[256];
        static int logBufferIndex = 0;
        static const char *TAG = "QUL";

        if (character != '\n') {
            logBuffer[logBufferIndex++] = character;
        }

        if (character == 0 || character == '\n' || logBufferIndex == sizeof(logBuffer) - 1) {
            logBuffer[logBufferIndex] = 0;
            ESP_LOGI(TAG, "%s", logBuffer);
            logBufferIndex = 0;
        }
    }

private:
    std::minstd_rand m_randomEngine;
};

PlatformContext *getPlatformInstance()
{
    static Esp32Platform platform;
    return &platform;
}

} // namespace Platform
} // namespace Qul

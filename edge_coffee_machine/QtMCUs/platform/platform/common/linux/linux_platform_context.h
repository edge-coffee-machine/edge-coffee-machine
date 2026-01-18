// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "platform_config.h"

#include <platform/platform.h>
#include <platform/alloc.h>

namespace Qul {
namespace Platform {

struct LinuxPerformanceMetrics : PerformanceMetrics
{
    uint64_t maxHeapUsage() override;
    uint64_t maxStackUsage() override;
    float cpuLoad() override;
};

struct LinuxPlatform : PlatformContext
{
    double rand() override;
    uint64_t update() override;
    uint64_t currentTimestamp() override;
    void exec() override;
    void initializeHardware() override;
    void initializePlatform() override;
    void scheduleEngineUpdate(uint64_t timestamp) override;

    void consoleWrite(char character) override;

    FrameBufferingType frameBufferingType(const PlatformInterface::LayerEngine::ItemLayer *) const override;
    FrameStatistics presentFrame(const PlatformInterface::Screen *screen, const PlatformInterface::Rect &rect) override;
    Qul::PlatformInterface::Screen *availableScreens(size_t *screenCount) const override;
    PlatformInterface::DrawingDevice *beginFrame(const PlatformInterface::LayerEngine::ItemLayer *layer,
                                                 const PlatformInterface::Rect &rect,
                                                 int refreshInterval) override;
    void endFrame(const PlatformInterface::LayerEngine::ItemLayer *layer) override;
    void initializeDisplay(const PlatformInterface::Screen *screen) override;

    PerformanceMetrics *performanceMetrics() override;
};

} // namespace Platform
} // namespace Qul

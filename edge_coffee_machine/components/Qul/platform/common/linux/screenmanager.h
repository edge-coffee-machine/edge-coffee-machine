// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#pragma once

#include <platforminterface/rect.h>
#include <platforminterface/screen.h>
#include <platforminterface/drawingdevice.h>
#include <platforminterface/drawingengine.h>
#include <platforminterface/platforminterface.h>
#include <platforminterface/log.h>
#include <platforminterface/error.h>

#include <platform/platform.h>

namespace Qul {
namespace Platform {

class ScreenManager
{
public:
    virtual FrameBufferingType frameBufferingType(const PlatformInterface::LayerEngine::ItemLayer *) const = 0;
    virtual Qul::PlatformInterface::Screen *availableScreens(size_t *screenCount) const = 0;
    virtual void initializeDisplay(const PlatformInterface::Screen *screen) = 0;
    virtual PlatformInterface::DrawingDevice *beginFrame(const PlatformInterface::LayerEngine::ItemLayer *layer,
                                                         const PlatformInterface::Rect &rect,
                                                         int refreshInterval)
        = 0;
    virtual void endFrame(const PlatformInterface::LayerEngine::ItemLayer *layer) = 0;

    virtual FrameStatistics presentFrame(const PlatformInterface::Screen *screen, const PlatformInterface::Rect &rect)
        = 0;
};

ScreenManager *screenManager();

} // namespace Platform
} // namespace Qul

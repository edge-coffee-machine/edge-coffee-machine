// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#pragma once

#include <platforminterface/platforminterface.h>
#include <platforminterface/drawingdevice.h>
#include <platforminterface/drawingengine.h>

namespace Qul {
namespace Platform {
namespace Private {
class PpaDrawingEngine : public PlatformInterface::DrawingEngine
{
public:
    PpaDrawingEngine();

    void blendRect(PlatformInterface::DrawingDevice *drawingDevice,
                   const PlatformInterface::Rect &rect,
                   PlatformInterface::Rgba32 color,
                   BlendMode blendMode) override;

    void blendImage(PlatformInterface::DrawingDevice *drawingDevice,
                    const PlatformInterface::Point &pos,
                    const PlatformInterface::Texture &source,
                    const PlatformInterface::Rect &sourceRect,
                    int sourceOpacity,
                    BlendMode blendMode) override;

    void blendAlphaMap(PlatformInterface::DrawingDevice *drawingDevice,
                       const PlatformInterface::Point &pos,
                       const PlatformInterface::Texture &source,
                       const PlatformInterface::Rect &sourceRect,
                       PlatformInterface::Rgba32 color,
                       BlendMode blendMode) override;

    void synchronizeForCpuAccess(PlatformInterface::DrawingDevice *drawingDevice,
                                 const PlatformInterface::Rect &rect) override;

    static void init();
    static void beginFrame(void *buffer, int width, int height, int bytesPerLine, PixelFormat format);
    static void flush();
};
} // namespace Private
} // namespace Platform
} // namespace Qul

// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#pragma once

#include "platform_config.h"

#include <platform/platform.h>
#include <qul/pixelformat.h>

#include <esp_lcd_types.h>

#include <cstdint>

#ifdef QUL_PLATFORM_DEVICELINK_ENABLED
#define QUL_PLATFORM_ESP32_SINGLE_BUFFERING_WITH_SPLIT_TRANSFER
#endif

namespace Qul {
namespace Platform {
namespace Private {

bool lcdFlushReady(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);
void flushLcd(const PlatformInterface::Rect &rect, void *data);
void initializeLcd();

uint8_t lcdBytesPerPixel();
uint16_t lcdHeight();
uint16_t lcdWidth();

Qul::PixelFormat lcdPixelFormat();

uchar *lcdFrameBuffer();
size_t lcdFrameBufferPixelCount();
FrameBufferingType frameBufferingType();

} // namespace Private
} // namespace Platform
} // namespace Qul

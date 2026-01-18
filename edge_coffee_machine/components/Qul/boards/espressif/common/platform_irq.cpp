// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include "platform_irq.h"
#include "platform_display.h"
#include "platform_touch.h"

extern "C" {
bool qulOnColorTransDoneHandler(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    return Qul::Platform::Private::lcdFlushReady(panel_io, edata, user_ctx);
}

void qulTouchIsrHandler(esp_lcd_touch_handle_t tp)
{
    Qul::Platform::Private::touchIsrHandler(tp);
}
}

// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#pragma once

#include <esp_lcd_touch.h>
#include <esp_lcd_types.h>

extern "C" {
bool qulOnColorTransDoneHandler(esp_lcd_panel_io_handle_t panel_io,
                                esp_lcd_panel_io_event_data_t *edata,
                                void *user_ctx);
void qulTouchIsrHandler(esp_lcd_touch_handle_t tp);
}

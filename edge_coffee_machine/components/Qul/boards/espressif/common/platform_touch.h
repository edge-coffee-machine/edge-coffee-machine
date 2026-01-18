// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include <esp_lcd_touch.h>

namespace Qul {
namespace Platform {
namespace Private {

void touchInit();
void touchIsrHandler(esp_lcd_touch_handle_t tp);
void touchRead();

} // namespace Private
} // namespace Platform
} // namespace Qul

// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum { MAINLOOP_SEMAPHORE, LCD_BLOCK_FLUSH_SEMAPHORE } SemaphoreType;

void initSuspension();
void suspend(SemaphoreType semaphoreType, uint64_t timeout);
void resume(SemaphoreType semaphoreType);

#ifdef __cplusplus
}
#endif
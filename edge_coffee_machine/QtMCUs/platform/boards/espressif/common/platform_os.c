// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include "platform_os.h"

#include <FreeRTOS.h>
#include <semphr.h>
#include <portmacro.h>
#include "FreeRTOSConfig.h"

#include <stdint.h>
#include <limits.h>

static SemaphoreHandle_t mainloopSemaphore = NULL;
static SemaphoreHandle_t lcdBlockFlushSemaphore = NULL;

void initSuspension()
{
    mainloopSemaphore = xSemaphoreCreateBinary();
    lcdBlockFlushSemaphore = xSemaphoreCreateBinary();
}

void suspend(SemaphoreType semaphoreType, uint64_t timeout)
{
    SemaphoreHandle_t semaphore;
    switch (semaphoreType) {
    case MAINLOOP_SEMAPHORE:
        semaphore = mainloopSemaphore;
        break;
    case LCD_BLOCK_FLUSH_SEMAPHORE:
        semaphore = lcdBlockFlushSemaphore;
        break;
    default:
        semaphore = NULL;
    }

    TickType_t timeoutTicks = 0;

#if configUSE_16_BIT_TICKS == 1
    timeoutTicks = (timeout >= USHRT_MAX) ? portMAX_DELAY : (timeout / portTICK_PERIOD_MS);
#else
    timeoutTicks = (timeout >= UINT_MAX) ? portMAX_DELAY : (timeout / portTICK_PERIOD_MS);
#endif

    if (semaphore)
        xSemaphoreTake(semaphore, timeoutTicks);
}

void resume(SemaphoreType semaphoreType)
{
    SemaphoreHandle_t semaphore;
    switch (semaphoreType) {
    case MAINLOOP_SEMAPHORE:
        semaphore = mainloopSemaphore;
        break;
    case LCD_BLOCK_FLUSH_SEMAPHORE:
        semaphore = lcdBlockFlushSemaphore;
        break;
    default:
        semaphore = NULL;
    }

    if (semaphore) {
        if (xPortInIsrContext() == pdTRUE)
            xSemaphoreGiveFromISR(semaphore, NULL);
        else
            xSemaphoreGive(semaphore);
    }
}

// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include <platforminterface/log.h>
#include <platform/private/mem_info.h>
#include <platform/platform.h>

#include "esp_task.h"

#include <freertos/FreeRTOS.h>
#include "task.h"

#include <utility>

#ifdef QUL_ENABLE_PERFORMANCE_LOGGING
#ifndef QUL_STACK_SIZE
#error QUL_STACK_SIZE must be defined for performance logging.
#endif
#if (INCLUDE_uxTaskGetStackHighWaterMark != 1)
#error uxTaskGetStackHighWaterMark must be enabled for performance logging. Please include "#define INCLUDE_uxTaskGetStackHighWaterMark 1" in your FreeRTOSConfig.h.
#endif
#endif

namespace Qul {
namespace Platform {
namespace Private {
uint32_t getStackUsage(void)
{
#ifdef QUL_ENABLE_PERFORMANCE_LOGGING
    /* UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask)
     *
     * Returns: The smallest amount of free stack space there has been
     * (in bytes not words, unlike vanilla FreeRTOS) since the task
     * referenced by xTask was created.
     *
     * See https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/api-reference/system/freertos.html#_CPPv427uxTaskGetStackHighWaterMark12TaskHandle_t
     */
    const UBaseType_t waterMark = uxTaskGetStackHighWaterMark(NULL);

    return ((uint32_t) QUL_STACK_SIZE - (uint32_t) waterMark);
#else
    return 0;
#endif
}

namespace FreeRtos {
void printHeapStats(void)
{
    const uint32_t sramAllocCaps = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;
    const uint32_t psramAllocCaps = MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT;

    Qul::PlatformInterface::log("SRAM heap free size: %d KB\r\n"
                                "SRAM heap minimum free size: %d KB\r\n"
                                "PSRAM heap free size: %d KB\r\n"
                                "PSRAM heap minimum free size: %d KB\r\n",
                                heap_caps_get_free_size(sramAllocCaps) / 1024,
                                heap_caps_get_minimum_free_size(sramAllocCaps) / 1024,
                                heap_caps_get_free_size(psramAllocCaps) / 1024,
                                heap_caps_get_minimum_free_size(psramAllocCaps) / 1024);
}

void printStackStats(void)
{
    Qul::PlatformInterface::log("Stack high water mark: %u bytes\r\n", uxTaskGetStackHighWaterMark(NULL));
}

void *allocate(std::size_t size)
{
    const uint32_t allocCaps = MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT;
    void *ptr = heap_caps_malloc(sizeof(size_t) + size, allocCaps);

    if (ptr == NULL) {
        return ptr;
    }

    *((size_t *) ptr) = size;

    return (size_t *) ptr + 1;
}

void deallocate(void *ptr)
{
    if (ptr != NULL) {
        heap_caps_free((size_t *) ptr - 1);
    }
}

void *reallocate(void *ptr, size_t s)
{
    if (ptr == NULL) {
        return allocate(s);
    }

    size_t oldSize = *((size_t *) ptr - 1);

    if (s == oldSize) {
        return ptr;
    }

    void *newPtr = NULL;

    if (s > 0) {
        newPtr = allocate(s);
        memcpy(newPtr, ptr, (s > oldSize) ? oldSize : s);
    }

    deallocate(ptr);

    return newPtr;
}

size_t allocationSize(void *ptr)
{
    if (!ptr)
        return 0;

    return *((size_t *) ptr - 1);
}

} // namespace FreeRtos
} // namespace Private
} // namespace Platform
} // namespace Qul

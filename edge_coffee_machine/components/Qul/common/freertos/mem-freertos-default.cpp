// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include <platform/mem.h>

#include <platform/private/mem-freertos.h>

namespace Qul {
namespace Platform {

void printHeapStats(void)
{
    Private::FreeRtos::printHeapStats();
}

void printStackStats(void)
{
    Private::FreeRtos::printStackStats();
}

void *qul_malloc(std::size_t size)
{
    return Private::FreeRtos::allocate(size);
}

void qul_free(void *ptr)
{
    Private::FreeRtos::deallocate(ptr);
}

void *qul_realloc(void *ptr, size_t size)
{
    return Private::FreeRtos::reallocate(ptr, size);
}

} // namespace Platform
} // namespace Qul

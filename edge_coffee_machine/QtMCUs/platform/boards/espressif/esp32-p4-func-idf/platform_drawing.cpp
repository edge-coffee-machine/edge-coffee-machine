// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include "platform/platform.h"

#include "platform_drawing.h"
#include "platform_os.h"

#include <esp_log.h>

namespace Qul {
namespace Platform {
namespace Private {
void swapBuffers();
bool swapBuffersPending();

unsigned char *virtualBackBuffer();
unsigned char *getPendingBuffer();

int flushFrame(int interval, int *remainingBudget)
{
    QUL_UNUSED(interval);

    while (swapBuffersPending()) {
        suspend(MAINLOOP_SEMAPHORE, 10);
    }

    swapBuffers();

    if (remainingBudget) {
        *remainingBudget = 1000;
    }

    return 0;
}

unsigned char *frontBuffer()
{
    return getPendingBuffer();
}

unsigned char *backBuffer()
{
    while (swapBuffersPending()) {
        suspend(MAINLOOP_SEMAPHORE, 10);
    }

    return virtualBackBuffer();
}

} // namespace Private
} // namespace Platform
} // namespace Qul

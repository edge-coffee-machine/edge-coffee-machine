// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#pragma once

namespace Qul {
namespace Platform {
namespace Private {
int flushFrame(int interval, int *remainingBudget);
unsigned char *frontBuffer();
unsigned char *backBuffer();
} // namespace Private
} // namespace Platform
} // namespace Qul

// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include <platform/private/freertosqueue.h>
#include <platform/mem.h>

#include <FreeRTOS.h>
#include <queue.h>

namespace Qul {
namespace Platform {
namespace Private {

FreeRTOSQueue::FreeRTOSQueue(uint32_t capacity, uint32_t messageSize)
    : MessageQueueInterface()
    , m_queue(xQueueCreate(static_cast<UBaseType_t>(capacity), static_cast<UBaseType_t>(messageSize)))
    , m_capacity(capacity)
    , m_overrun(false)
{}

FreeRTOSQueue::~FreeRTOSQueue()
{
    vQueueDelete(m_queue);
}

MessageQueueStatus FreeRTOSQueue::enqueueOrDiscard(const void *message)
{
    if (xQueueSendToBack(m_queue, message, 0) == pdTRUE) {
        return MessageQueueStatus::Success;
    } else {
        portDISABLE_INTERRUPTS();
        m_overrun = true;
        portENABLE_INTERRUPTS();
        return MessageQueueStatus::MessageDiscarded;
    }
}

MessageQueueStatus FreeRTOSQueue::enqueueOrDiscardFromInterrupt(const void *message)
{
    if (xQueueSendToBackFromISR(m_queue, message, NULL) == pdPASS) {
        return MessageQueueStatus::Success;
    } else {
        m_overrun = true;
        return MessageQueueStatus::MessageDiscarded;
    }
}

MessageQueueStatus FreeRTOSQueue::enqueueOrOverwrite(const void *message)
{
    (void) message;

    return MessageQueueStatus::OverwriteNotSupported;
}

bool FreeRTOSQueue::isOverrun() const
{
    return m_overrun;
}

void FreeRTOSQueue::clearOverrun()
{
    portDISABLE_INTERRUPTS();
    m_overrun = false;
    portENABLE_INTERRUPTS();
}

MessageQueueStatus FreeRTOSQueue::receive(void *message, int32_t timeout)
{
    BaseType_t receiveStatus;
    if (timeout < 0)
        receiveStatus = xQueueReceive(m_queue, message, portMAX_DELAY);
    else
        receiveStatus = xQueueReceive(m_queue, message, pdMS_TO_TICKS(timeout));

    if (receiveStatus == pdTRUE)
        return MessageQueueStatus::Success;
    else if (timeout != 0)
        return MessageQueueStatus::Timeout;
    else
        return MessageQueueStatus::EmptyQueue;
}

MessageQueueStatus FreeRTOSQueue::receiveFromInterrupt(void *message, int32_t timeout)
{
    (void) timeout;

    if (xQueueReceiveFromISR(m_queue, message, NULL) == pdTRUE)
        return MessageQueueStatus::Success;
    else
        return MessageQueueStatus::EmptyQueue;
}

bool FreeRTOSQueue::isEmpty() const
{
    return uxQueueMessagesWaiting(m_queue) == 0;
}

bool FreeRTOSQueue::isEmptyFromInterrupt() const
{
    return xQueueIsQueueEmptyFromISR(m_queue) == pdTRUE;
}

bool FreeRTOSQueue::discardSupported() const
{
    return true;
}

bool FreeRTOSQueue::overwriteSupported() const
{
    return false;
}

uint32_t FreeRTOSQueue::capacity() const
{
    return m_capacity;
}

} // namespace Private

MessageQueueInterface *requestQueue(size_t queueCapacity, size_t messageSize)
{
    // Using new is not allowed, so we manually allocate the needed space from
    // heap and then use placement new to properly place the class instance
    // and call its constructor.
    void *queue = qul_malloc(sizeof(Private::FreeRTOSQueue));

    if (queue == nullptr) {
        return nullptr;
    }

    MessageQueueInterface *interface = new (queue) Private::FreeRTOSQueue(queueCapacity, messageSize);
    return interface;
}

void deleteQueue(MessageQueueInterface *queue)
{
    Private::FreeRTOSQueue *fq = static_cast<Private::FreeRTOSQueue *>(queue);
    fq->~FreeRTOSQueue();
    qul_free(fq);
}

size_t maximumQueueMessageSize()
{
    return SIZE_MAX;
}

} // namespace Platform
} // namespace Qul

// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include "platform_irq.h"
#include "platform_os.h"

#include <qul/eventqueue.h>
#include <platform/platform.h>
#include <platform/singlepointtoucheventdispatcher.h>
#include <platforminterface/log.h>

#include <esp_lcd_touch.h>
#include <bsp/esp-bsp.h>
#include <esp_log.h>
#include <bsp/touch.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace Qul {
namespace Platform {
namespace Private {

using namespace Qul::PlatformInterface;

class SinglePointTouchEventQueue : public EventQueue<SinglePointTouchEvent>
{
public:
    void onEvent(const SinglePointTouchEvent &event) override { touchEventDispatcher.dispatch(event); }

private:
    SinglePointTouchEventDispatcher touchEventDispatcher;
};

static SinglePointTouchEventQueue touchEventQueue;

constexpr bool swapXY = false;
constexpr bool inverseX = true;
constexpr bool inverseY = false;
constexpr int lcdWidth = BSP_LCD_H_RES;
constexpr int lcdHeight = BSP_LCD_V_RES;

static uint16_t prevX = 0;
static uint16_t prevY = 0;
static uint16_t prevPressed = 0;
static uint16_t prevTouchPointnum = 0;
static esp_lcd_touch_handle_t touchHandle = nullptr;
static esp_timer_handle_t touchPollTimer = nullptr;

static void touchPollTimerCallback(void *arg)
{
    resume(MAINLOOP_SEMAPHORE);
}

void touchInit()
{
    const bsp_touch_config_t touchConfig = {};

    esp_err_t ret = bsp_touch_new(&touchConfig, &touchHandle);
    if (ret != ESP_OK) {
        return;
    }

    // Since ESP32-P4 doesn't have touch interrupt pin connected,
    // create a timer to periodically wake up the main loop for touch polling
    const esp_timer_create_args_t timer_args = {.callback = touchPollTimerCallback,
                                                .arg = nullptr,
                                                .dispatch_method = ESP_TIMER_TASK,
                                                .name = "touch_poll"};

    ret = esp_timer_create(&timer_args, &touchPollTimer);
    if (ret == ESP_OK) {
        // Start periodic timer with 16ms interval (60fps touch polling)
        esp_timer_start_periodic(touchPollTimer, 16000);
    }
}

void touchRead()
{
    if (!touchHandle) {
        return;
    }

    /* Use previous values as initial values. If we miss release event
     * reading from the driver we'll still provide correct X and Y for
     * the SinglePointTouchEvent.
     */
    uint16_t x = prevX;
    uint16_t y = prevY;
    uint8_t touchPointNum = 0;

    esp_err_t read_result = esp_lcd_touch_read_data(touchHandle);
    if (read_result != ESP_OK) {
        if (read_result != ESP_ERR_NOT_FOUND) {
            static int error_count = 0;
            if (error_count++ < 3) {
                ESP_LOGD("QUL", "Touch read error: %s", esp_err_to_name(read_result));
            }
        }
        return;
    }

    /* polling the touch controller to get the coordinates, this is needed
     * because the touch controller might not have new data available yet, and we need to
     * wait for it to be ready before reading the coordinates.
     * If the controller has no data, it will return ESP_ERR_NOT_FOUND, which we handle by
     * returning early and not posting an event.
    */
    if (!esp_lcd_touch_get_coordinates(touchHandle, &x, &y, nullptr, &touchPointNum, 1)) {
        if (prevPressed) {
            SinglePointTouchEvent event({getPlatformInstance()->currentTimestamp(), prevX, prevY, false});
            touchEventQueue.postEvent(event);
            prevPressed = 0;
        }
        return;
    }

    if ((prevX != x) || (prevY != y) || (prevTouchPointnum != touchPointNum)) {
        if ((prevTouchPointnum == 0) && (touchPointNum == 0)) {
            return;
        }

        SinglePointTouchEvent event;
        event.x = x;
        event.y = y;
        event.pressed = touchPointNum > 0;
        event.timestamp = getPlatformInstance()->currentTimestamp();

        touchEventQueue.postEvent(event);

        prevX = x;
        prevY = y;
        prevPressed = event.pressed;
        prevTouchPointnum = touchPointNum;
    }
}

} // namespace Private
} // namespace Platform
} // namespace Qul

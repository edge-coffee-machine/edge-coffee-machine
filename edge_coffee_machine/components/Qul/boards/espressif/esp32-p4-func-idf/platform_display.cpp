// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include "platform_display.h"
#include "platform_irq.h"
#include "platform_os.h"

#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_mipi_dsi.h>
#include <bsp/esp-bsp.h>
#include <bsp/display.h>

#include <esp_timer.h>
#include <esp_log.h>
#include <esp_heap_caps.h>
#include <rom/ets_sys.h>
#include <cstring>

namespace Qul {
namespace Platform {
namespace Private {

static uchar *framebuffers[2] = {nullptr, nullptr};
static esp_lcd_panel_handle_t panelHandle = nullptr;
static esp_lcd_panel_io_handle_t ioHandle = nullptr;
static SemaphoreHandle_t flushDone = nullptr;

static volatile uint8_t currentBackbuffer = 0;
static volatile bool waitingForFlip = false;
static volatile uchar *pendingBuffer = nullptr;

static void swapEndianness(const PlatformInterface::Rect &rect, uint16_t *src, uint16_t *dst)
{
    auto bits = src;
    for (int row = rect.top(); row <= rect.bottom(); row++) {
        int col = rect.left();

        for (; col <= rect.right(); col++, bits++, dst++) {
            uint16_t content = *bits;
            *dst = ((content & 0x00FF) << 8) | ((content & 0xFF00) >> 8);
        }
    }
}

bool lcdFlushReady(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    (void) panel_io;
    (void) edata;
    (void) user_ctx;

    waitingForFlip = false;

    if (flushDone != nullptr) {
        xSemaphoreGiveFromISR(flushDone, NULL);
    }
    return false;
}

unsigned char *virtualBackBuffer()
{
    if (!framebuffers[0]) {
        lcdFrameBuffer();
    }

    if (!framebuffers[0] || !framebuffers[1]) {
        ESP_LOGE("DISPLAY", "framebuffers not allocated!");
        return nullptr;
    }
    unsigned char *buffer = reinterpret_cast<unsigned char *>(framebuffers[currentBackbuffer]);
    ESP_LOGD("DISPLAY", "virtualBackBuffer: buffer=%p, index=%d", buffer, currentBackbuffer);
    return buffer;
}

unsigned char *virtualFrontBuffer()
{
    if (!framebuffers[0]) {
        lcdFrameBuffer();
    }

    if (!framebuffers[0] || !framebuffers[1]) {
        ESP_LOGE("DISPLAY", "framebuffers not allocated!");
        return nullptr;
    }
    unsigned char *buffer = reinterpret_cast<unsigned char *>(framebuffers[!currentBackbuffer]);
    ESP_LOGI("DISPLAY", "virtualFrontBuffer: buffer=%p, index=%d", buffer, !currentBackbuffer);
    return buffer;
}

bool swapBuffersPending()
{
    return waitingForFlip;
}

void swapBuffers()
{
    unsigned char *buffer = virtualBackBuffer();
    currentBackbuffer = !currentBackbuffer;

    pendingBuffer = buffer;
    waitingForFlip = true;

    ESP_LOGD("DISPLAY", "swapBuffers: pending=%p, new back index=%d", buffer, currentBackbuffer);
}

unsigned char *getPendingBuffer()
{
    return const_cast<unsigned char *>(pendingBuffer);
}

void flushLcd(const PlatformInterface::Rect &rect, void *data)
{
    ESP_LOGD("DISPLAY", "flushLcd: rect=(%d,%d,%dx%d), data=%p", rect.x(), rect.y(), rect.width(), rect.height(), data);

    if (pdPASS != xSemaphoreTake(flushDone, pdMS_TO_TICKS(1000))) {
        return;
    }

#if (BSP_LCD_BIGENDIAN == 1)
    /* Swap endianness in place for avoiding extra buffer usage as we don't need to
       keep the raw data.
    */
    auto pixels = reinterpret_cast<uint16_t *>(data);
    swapEndianness(rect, pixels, pixels);
#endif

    ESP_ERROR_CHECK(
        esp_lcd_panel_draw_bitmap(panelHandle, rect.left(), rect.top(), rect.right() + 1, rect.bottom() + 1, data));
}

void initializeLcd()
{
    flushDone = xSemaphoreCreateBinary();
    xSemaphoreGive(flushDone);

    ESP_ERROR_CHECK(bsp_display_brightness_init());

    const bsp_display_config_t displayConfig = {
        .hdmi_resolution = BSP_HDMI_RES_NONE,
        .dsi_bus = {
            .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
            .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
        },
    };

    ESP_ERROR_CHECK(bsp_display_new(&displayConfig, &panelHandle, &ioHandle));

    const esp_lcd_dpi_panel_event_callbacks_t callback = {
        .on_color_trans_done = qulOnColorTransDoneHandler,
    };
    esp_lcd_dpi_panel_register_event_callbacks(panelHandle, &callback, NULL);
    ESP_ERROR_CHECK(bsp_display_backlight_on());

    lcdFrameBuffer();
}

uint8_t lcdBytesPerPixel()
{
    return BSP_LCD_BITS_PER_PIXEL / 8;
}

uint16_t lcdHeight()
{
    return BSP_LCD_V_RES;
}

uint16_t lcdWidth()
{
    return BSP_LCD_H_RES;
}

Qul::PixelFormat lcdPixelFormat()
{
#if (BSP_LCD_COLOR_FORMAT == ESP_LCD_COLOR_FORMAT_RGB565)
    return Qul::PixelFormat_RGB16;
#elif (BSP_LCD_COLOR_FORMAT == ESP_LCD_COLOR_FORMAT_RGB888)
    return Qul::PixelFormat_RGB888;
#else
#error "Unsupported pixel format"
#endif
}

uchar *lcdFrameBuffer()
{
    if (!framebuffers[0]) {
        size_t pixelCount = lcdFrameBufferPixelCount();
        size_t bytesPerPixel = lcdBytesPerPixel();
        size_t size = pixelCount * bytesPerPixel;

        size_t alignedSize = ((size + 63) & ~63);
        size_t total = alignedSize * 2;

        uint32_t buf_alloc_caps = MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT;
        uchar *blk = static_cast<uchar *>(heap_caps_aligned_alloc(64, total, buf_alloc_caps));
        QUL_ASSERT(blk != nullptr, QulError_Platform_MemoryAllocationFailed);

        framebuffers[0] = blk;
        framebuffers[1] = blk + alignedSize;
        currentBackbuffer = 1;
        memset(framebuffers[0], 0, alignedSize);
        memset(framebuffers[1], 0, alignedSize);

        ESP_LOGI("DISPLAY",
                 "Allocated framebuffers: %p, %p (size: %zu each)",
                 framebuffers[0],
                 framebuffers[1],
                 alignedSize);
    }

    return virtualBackBuffer();
}

size_t lcdFrameBufferPixelCount()
{
    return lcdWidth() * lcdHeight();
}

} // namespace Private
} // namespace Platform
} // namespace Qul

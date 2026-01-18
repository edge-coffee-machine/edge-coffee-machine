// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include "ppadrawingengine.h"
#include <platforminterface/drawingdevice.h>
#include <platforminterface/rect.h>
#include <platforminterface/point.h>
#include <platforminterface/rgba32.h>
#include <platforminterface/texture.h>
#include <platforminterface/transform.h>
#include <qul/pixelformat.h>
#include <qul/private/global.h>

#include <driver/ppa.h>
#include <esp_log.h>
#include <esp_cache.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <cstring>
#include <cmath>

namespace Qul {
namespace Platform {
namespace Private {

static const char *TAG = "PPA_DRAWING_ENGINE";

#define CACHE_LINE_SIZE 64
#define CACHE_ALIGN_UP(size) (((size) + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1))
#define IS_CACHE_ALIGNED(addr) (((uintptr_t) (addr) & (CACHE_LINE_SIZE - 1)) == 0)

static ppa_client_handle_t s_ppaFillClient = nullptr;
static ppa_client_handle_t s_ppaBlendClient = nullptr;
static ppa_client_handle_t s_ppaSrmClient = nullptr;
static SemaphoreHandle_t s_ppaSemaphore = nullptr;

static void *s_currentFramebuffer = nullptr;
static int s_currentWidth = 0;
static int s_currentHeight = 0;
static int s_currentBytesPerLine = 0;
static PixelFormat s_currentPixelFormat = PixelFormat_Invalid;

PpaDrawingEngine::PpaDrawingEngine() {}

static bool IRAM_ATTR ppa_transaction_done_cb(ppa_client_handle_t ppa_client,
                                              ppa_event_data_t *event_data,
                                              void *user_data)
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    SemaphoreHandle_t semaphore = static_cast<SemaphoreHandle_t>(user_data);
    xSemaphoreGiveFromISR(semaphore, &higherPriorityTaskWoken);
    return higherPriorityTaskWoken == pdTRUE;
}

static ppa_srm_color_mode_t qtToSrmColorMode(PixelFormat format)
{
    switch (format) {
    case PixelFormat_ARGB32:
    case PixelFormat_ARGB32_Premultiplied:
        return PPA_SRM_COLOR_MODE_ARGB8888;
    case PixelFormat_RGB32:
        return PPA_SRM_COLOR_MODE_RGB888;
    case PixelFormat_RGB16:
        return PPA_SRM_COLOR_MODE_RGB565;
    default:
        return PPA_SRM_COLOR_MODE_ARGB8888;
    }
}

static ppa_blend_color_mode_t qtToBlendColorMode(PixelFormat format)
{
    switch (format) {
    case PixelFormat_ARGB32:
    case PixelFormat_ARGB32_Premultiplied:
        return PPA_BLEND_COLOR_MODE_ARGB8888;
    case PixelFormat_RGB32:
        return PPA_BLEND_COLOR_MODE_RGB888;
    case PixelFormat_RGB16:
        return PPA_BLEND_COLOR_MODE_RGB565;
    default:
        return PPA_BLEND_COLOR_MODE_ARGB8888;
    }
}

static ppa_fill_color_mode_t qtToFillColorMode(PixelFormat format)
{
    switch (format) {
    case PixelFormat_ARGB32:
    case PixelFormat_ARGB32_Premultiplied:
        return PPA_FILL_COLOR_MODE_ARGB8888;
    case PixelFormat_RGB32:
        return PPA_FILL_COLOR_MODE_RGB888;
    case PixelFormat_RGB16:
        return PPA_FILL_COLOR_MODE_RGB565;
    default:
        return PPA_FILL_COLOR_MODE_ARGB8888;
    }
}

static color_pixel_argb8888_data_t qtRgba32ToPpaArgb8888(PlatformInterface::Rgba32 color)
{
    color_pixel_argb8888_data_t ppaColor = {};
    ppaColor.a = color.alpha();
    ppaColor.r = color.red();
    ppaColor.g = color.green();
    ppaColor.b = color.blue();
    return ppaColor;
}

void PpaDrawingEngine::init()
{
    s_ppaSemaphore = xSemaphoreCreateBinary();
    QUL_ASSERT(s_ppaSemaphore != nullptr, QulError_Platform_MemoryAllocationFailed);
    xSemaphoreGive(s_ppaSemaphore);

    ppa_client_config_t fillClientConfig = {.oper_type = PPA_OPERATION_FILL,
                                            .max_pending_trans_num = 1,
                                            .data_burst_length = PPA_DATA_BURST_LENGTH_16};

    ppa_client_config_t blendClientConfig = {.oper_type = PPA_OPERATION_BLEND,
                                             .max_pending_trans_num = 1,
                                             .data_burst_length = PPA_DATA_BURST_LENGTH_16};

    ppa_client_config_t srmClientConfig = {.oper_type = PPA_OPERATION_SRM,
                                           .max_pending_trans_num = 1,
                                           .data_burst_length = PPA_DATA_BURST_LENGTH_16};

    esp_err_t ret = ppa_register_client(&fillClientConfig, &s_ppaFillClient);
    QUL_ASSERT(ret == ESP_OK, QulError_DisplayDriver_SdkError);

    ret = ppa_register_client(&blendClientConfig, &s_ppaBlendClient);
    QUL_ASSERT(ret == ESP_OK, QulError_DisplayDriver_SdkError);

    ret = ppa_register_client(&srmClientConfig, &s_ppaSrmClient);
    QUL_ASSERT(ret == ESP_OK, QulError_DisplayDriver_SdkError);

    ppa_event_callbacks_t callbacks = {.on_trans_done = ppa_transaction_done_cb};

    ppa_client_register_event_callbacks(s_ppaFillClient, &callbacks);
    ppa_client_register_event_callbacks(s_ppaBlendClient, &callbacks);
    ppa_client_register_event_callbacks(s_ppaSrmClient, &callbacks);
}

void PpaDrawingEngine::beginFrame(void *buffer, int width, int height, int bytesPerLine, PixelFormat format)
{
    s_currentFramebuffer = buffer;
    s_currentWidth = width;
    s_currentHeight = height;
    s_currentBytesPerLine = bytesPerLine;
    s_currentPixelFormat = format;

    if (buffer) {
        size_t alignedBufferSize = CACHE_ALIGN_UP(height * bytesPerLine);
        esp_cache_msync(buffer, alignedBufferSize, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    }
}

void PpaDrawingEngine::flush()
{
    if (xSemaphoreTake(s_ppaSemaphore, portMAX_DELAY) == pdTRUE) {
        xSemaphoreGive(s_ppaSemaphore);
    }
}

void PpaDrawingEngine::blendRect(PlatformInterface::DrawingDevice *drawingDevice,
                                 const PlatformInterface::Rect &rect,
                                 PlatformInterface::Rgba32 color,
                                 BlendMode blendMode)
{
    if (xSemaphoreTake(s_ppaSemaphore, portMAX_DELAY) != pdTRUE) {
        return;
    }

    // For transparent colors or non-SourceOver blend modes, fall back to CPU engine
    // to ensure proper blending with background content
    if (blendMode != BlendMode_SourceOver || color.alpha() < 255) {
        xSemaphoreGive(s_ppaSemaphore);
        drawingDevice->fallbackDrawingEngine()->blendRect(drawingDevice, rect, color, blendMode);
        return;
    }

    size_t alignedBufferSize = CACHE_ALIGN_UP(s_currentHeight * s_currentBytesPerLine);

    ppa_fill_oper_config_t fillConfig = {};
    fillConfig.out.buffer = s_currentFramebuffer;
    fillConfig.out.buffer_size = alignedBufferSize;
    fillConfig.out.pic_w = s_currentWidth;
    fillConfig.out.pic_h = s_currentHeight;
    fillConfig.out.block_offset_x = rect.x();
    fillConfig.out.block_offset_y = rect.y();
    fillConfig.out.fill_cm = qtToFillColorMode(s_currentPixelFormat);
    fillConfig.fill_block_w = rect.width();
    fillConfig.fill_block_h = rect.height();
    fillConfig.fill_argb_color = qtRgba32ToPpaArgb8888(color);
    fillConfig.mode = PPA_TRANS_MODE_NON_BLOCKING;
    fillConfig.user_data = s_ppaSemaphore;

    esp_err_t ret = ppa_do_fill(s_ppaFillClient, &fillConfig);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PPA fill operation failed: %d", ret);
        xSemaphoreGive(s_ppaSemaphore);
    }
}

void PpaDrawingEngine::blendImage(PlatformInterface::DrawingDevice *drawingDevice,
                                  const PlatformInterface::Point &pos,
                                  const PlatformInterface::Texture &source,
                                  const PlatformInterface::Rect &sourceRect,
                                  int sourceOpacity,
                                  BlendMode blendMode)
{
    if (xSemaphoreTake(s_ppaSemaphore, portMAX_DELAY) != pdTRUE) {
        return;
    }

    if (source.format() != s_currentPixelFormat) {
        xSemaphoreGive(s_ppaSemaphore);
        drawingDevice->fallbackDrawingEngine()
            ->blendImage(drawingDevice, pos, source, sourceRect, sourceOpacity, blendMode);
        return;
    }

    const void *sourceData = source.data();
    size_t alignedDestBufferSize = CACHE_ALIGN_UP(s_currentHeight * s_currentBytesPerLine);
    size_t alignedSrcBufferSize = CACHE_ALIGN_UP(source.height() * source.bytesPerLine());

    ppa_blend_oper_config_t blendConfig = {};

    blendConfig.in_bg.buffer = s_currentFramebuffer;
    blendConfig.in_bg.pic_w = s_currentWidth;
    blendConfig.in_bg.pic_h = s_currentHeight;
    blendConfig.in_bg.block_w = sourceRect.width();
    blendConfig.in_bg.block_h = sourceRect.height();
    blendConfig.in_bg.block_offset_x = pos.x();
    blendConfig.in_bg.block_offset_y = pos.y();
    blendConfig.in_bg.blend_cm = qtToBlendColorMode(s_currentPixelFormat);

    blendConfig.in_fg.buffer = const_cast<void *>(sourceData);
    blendConfig.in_fg.pic_w = source.width();
    blendConfig.in_fg.pic_h = source.height();
    blendConfig.in_fg.block_w = sourceRect.width();
    blendConfig.in_fg.block_h = sourceRect.height();
    blendConfig.in_fg.block_offset_x = sourceRect.x();
    blendConfig.in_fg.block_offset_y = sourceRect.y();
    blendConfig.in_fg.blend_cm = qtToBlendColorMode(source.format());

    blendConfig.out.buffer = s_currentFramebuffer;
    blendConfig.out.buffer_size = alignedDestBufferSize;
    blendConfig.out.pic_w = s_currentWidth;
    blendConfig.out.pic_h = s_currentHeight;
    blendConfig.out.block_offset_x = pos.x();
    blendConfig.out.block_offset_y = pos.y();
    blendConfig.out.blend_cm = qtToBlendColorMode(s_currentPixelFormat);

    blendConfig.bg_ck_en = false;
    blendConfig.fg_ck_en = false;
    blendConfig.mode = PPA_TRANS_MODE_NON_BLOCKING;
    blendConfig.user_data = s_ppaSemaphore;

    if (sourceOpacity < 255) {
        blendConfig.fg_alpha_update_mode = PPA_ALPHA_FIX_VALUE;
        blendConfig.fg_alpha_fix_val = sourceOpacity;

        if (blendMode == BlendMode_Source) {
            blendConfig.bg_alpha_update_mode = PPA_ALPHA_FIX_VALUE;
            blendConfig.bg_alpha_fix_val = 0;
        } else {
            blendConfig.bg_alpha_update_mode = PPA_ALPHA_INVERT;
        }
    } else {
        blendConfig.fg_alpha_update_mode = PPA_ALPHA_NO_CHANGE;
        blendConfig.bg_alpha_update_mode = PPA_ALPHA_NO_CHANGE;
    }

    esp_err_t ret = ppa_do_blend(s_ppaBlendClient, &blendConfig);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PPA blend operation failed: %d", ret);
        xSemaphoreGive(s_ppaSemaphore);
    }
}

void PpaDrawingEngine::blendAlphaMap(PlatformInterface::DrawingDevice *drawingDevice,
                                     const PlatformInterface::Point &pos,
                                     const PlatformInterface::Texture &source,
                                     const PlatformInterface::Rect &sourceRect,
                                     PlatformInterface::Rgba32 color,
                                     BlendMode blendMode)
{
    if (xSemaphoreTake(s_ppaSemaphore, portMAX_DELAY) != pdTRUE) {
        return;
    }

    if (source.format() == PixelFormat_Alpha8 || source.format() == PixelFormat_Alpha1) {
        xSemaphoreGive(s_ppaSemaphore);
        drawingDevice->fallbackDrawingEngine()->blendAlphaMap(drawingDevice, pos, source, sourceRect, color, blendMode);
        return;
    }

    const void *sourceData = source.data();
    size_t alignedDestBufferSize = CACHE_ALIGN_UP(s_currentHeight * s_currentBytesPerLine);
    ppa_blend_oper_config_t blendConfig = {};

    blendConfig.in_bg.buffer = s_currentFramebuffer;
    blendConfig.in_bg.pic_w = s_currentWidth;
    blendConfig.in_bg.pic_h = s_currentHeight;
    blendConfig.in_bg.block_w = sourceRect.width();
    blendConfig.in_bg.block_h = sourceRect.height();
    blendConfig.in_bg.block_offset_x = pos.x();
    blendConfig.in_bg.block_offset_y = pos.y();
    blendConfig.in_bg.blend_cm = qtToBlendColorMode(s_currentPixelFormat);

    blendConfig.in_fg.buffer = const_cast<void *>(sourceData);
    blendConfig.in_fg.pic_w = source.width();
    blendConfig.in_fg.pic_h = source.height();
    blendConfig.in_fg.block_w = sourceRect.width();
    blendConfig.in_fg.block_h = sourceRect.height();
    blendConfig.in_fg.block_offset_x = sourceRect.x();
    blendConfig.in_fg.block_offset_y = sourceRect.y();

    blendConfig.out.buffer = s_currentFramebuffer;
    blendConfig.out.buffer_size = alignedDestBufferSize;
    blendConfig.out.pic_w = s_currentWidth;
    blendConfig.out.pic_h = s_currentHeight;
    blendConfig.out.block_offset_x = pos.x();
    blendConfig.out.block_offset_y = pos.y();
    blendConfig.out.blend_cm = qtToBlendColorMode(s_currentPixelFormat);

    blendConfig.fg_fix_rgb_val.r = color.red();
    blendConfig.fg_fix_rgb_val.g = color.green();
    blendConfig.fg_fix_rgb_val.b = color.blue();

    if (source.format() == PixelFormat_Alpha8) {
        blendConfig.in_fg.blend_cm = PPA_BLEND_COLOR_MODE_A8;
    }

    if (color.alpha() < 255) {
        blendConfig.fg_alpha_update_mode = PPA_ALPHA_FIX_VALUE;
        blendConfig.fg_alpha_fix_val = color.alpha();

        if (blendMode == BlendMode_Source) {
            blendConfig.bg_alpha_update_mode = PPA_ALPHA_FIX_VALUE;
            blendConfig.bg_alpha_fix_val = 0;
        } else {
            blendConfig.bg_alpha_update_mode = PPA_ALPHA_INVERT;
        }
    } else {
        blendConfig.fg_alpha_update_mode = PPA_ALPHA_NO_CHANGE;
        blendConfig.bg_alpha_update_mode = PPA_ALPHA_NO_CHANGE;
    }

    blendConfig.bg_ck_en = false;
    blendConfig.fg_ck_en = false;
    blendConfig.mode = PPA_TRANS_MODE_NON_BLOCKING;
    blendConfig.user_data = s_ppaSemaphore;

    esp_err_t ret = ppa_do_blend(s_ppaBlendClient, &blendConfig);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PPA alpha map blend operation failed: %d", ret);
        xSemaphoreGive(s_ppaSemaphore);
    }
}

void PpaDrawingEngine::synchronizeForCpuAccess(PlatformInterface::DrawingDevice *drawingDevice,
                                               const PlatformInterface::Rect &rect)
{
    QUL_UNUSED(drawingDevice);
    QUL_UNUSED(rect);

    if (s_ppaSemaphore) {
        if (xSemaphoreTake(s_ppaSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
            xSemaphoreGive(s_ppaSemaphore);
        }
    }
}

} // namespace Private
} // namespace Platform
} // namespace Qul

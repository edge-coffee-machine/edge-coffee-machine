#include "who_p4_cam.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_video_device.h"
#include "esp_video_init.h"

#include "bsp/esp-bsp.h"

#include <fcntl.h>

static const char *TAG = "WhoP4Cam";

namespace who {
namespace cam {

WhoP4Cam::WhoP4Cam(const uint32_t v4l2_fmt,
                   const uint8_t fb_count,
                   const v4l2_memory fb_mem_type,
                   bool vertical_flip,
                   bool horizontal_flip) :
    WhoCam(fb_count), m_v4l2_fmt(v4l2_fmt), m_fb_mem_type(fb_mem_type)
{
    if (fb_count < 2) {
        ESP_LOGE(TAG, "fb_count is at least 2.");
    }
    if ((fb_mem_type != V4L2_MEMORY_MMAP) && (fb_mem_type != V4L2_MEMORY_USERPTR)) {
        ESP_LOGE(TAG, "unsupported buffer memory type.");
    }
    video_init(vertical_flip, horizontal_flip);
}

WhoP4Cam::~WhoP4Cam()
{
    video_deinit();
    for (int i = 0; i < m_fb_count; i++) {
        if (m_fb_mem_type == V4L2_MEMORY_USERPTR) {
            heap_caps_free(m_cam_fbs[i].buf);
        }
        delete (struct v4l2_buffer *)m_cam_fbs[i].ret;
    }
}

esp_err_t WhoP4Cam::set_flip(bool vertical_flip, bool horizontal_flip)
{
    if (!vertical_flip & !horizontal_flip) {
        return ESP_OK;
    }
    struct v4l2_ext_controls controls;
    controls.ctrl_class = V4L2_CTRL_CLASS_USER;
    struct v4l2_ext_control control[1];
    controls.count = 1;
    controls.controls = control;
    if (vertical_flip) {
        control[0].id = V4L2_CID_VFLIP;
        control[0].value = 1;
        if (ioctl(m_fd, VIDIOC_S_EXT_CTRLS, &controls) != 0) {
            ESP_LOGE(TAG, "Failed to mirror the frame vertically.");
            close(m_fd);
            return ESP_FAIL;
        }
    }
    if (horizontal_flip) {
        control[0].id = V4L2_CID_HFLIP;
        control[0].value = 1;
        if (ioctl(m_fd, VIDIOC_S_EXT_CTRLS, &controls) != 0) {
            ESP_LOGE(TAG, "Failed to mirror the frame horizontally.");
            close(m_fd);
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

void WhoP4Cam::video_init(bool vertical_flip, bool horizontal_flip)
{
    ESP_LOGI(TAG, "=== Starting WHO CAM VIDEO INIT ===");
    
    static bool once = []() {
        ESP_LOGI("WhoP4Cam", "Attempting to get I2C handle...");
        i2c_master_bus_handle_t i2c_bus_handle = bsp_i2c_get_handle();

        if (i2c_bus_handle == NULL) {
            ESP_LOGE("WhoP4Cam", "BSP I2C Handle is NULL");
            return false;
        }
        ESP_LOGI("WhoP4Cam", "I2C handle obtained successfully");

        // Verify camera is responding
        ESP_LOGI("WhoP4Cam", "Verifying camera at 0x30...");
        i2c_master_dev_handle_t probe_dev;
        i2c_device_config_t probe_cfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = 0x30,
            .scl_speed_hz = 400000,
        };
        
        bool camera_found = false;
        if (i2c_master_bus_add_device(i2c_bus_handle, &probe_cfg, &probe_dev) == ESP_OK) {
            uint8_t dummy;
            if (i2c_master_receive(probe_dev, &dummy, 1, 100) == ESP_OK) {
                ESP_LOGI("WhoP4Cam", "Camera responding at 0x30 ✓");
                camera_found = true;
            }
            i2c_master_bus_rm_device(probe_dev);
        }
        
        if (!camera_found) {
            ESP_LOGE("WhoP4Cam", "Camera not responding - check power sequence");
            return false;
        }
        
        esp_video_init_csi_config_t csi_config = {
            .sccb_config = {
                .init_sccb = false, 
                .i2c_handle = i2c_bus_handle,
                .freq = 400000,
            },
            .reset_pin    = GPIO_NUM_NC, 
            .pwdn_pin     = GPIO_NUM_NC, 
            .dont_init_ldo = false,
        };

        esp_video_init_config_t cam_config{};
        cam_config.csi = &csi_config;

        // Enable detailed logging
        esp_log_level_set("*", ESP_LOG_INFO);
        esp_log_level_set("sc2336", ESP_LOG_VERBOSE);
        esp_log_level_set("mipi_csi", ESP_LOG_VERBOSE);
        esp_log_level_set("esp_video", ESP_LOG_VERBOSE);
        esp_log_level_set("video_device", ESP_LOG_VERBOSE);
        esp_log_level_set("isp", ESP_LOG_VERBOSE);
        esp_log_level_set("isp_core", ESP_LOG_VERBOSE);
        esp_log_level_set("vfs", ESP_LOG_VERBOSE);

        ESP_LOGI("WhoP4Cam", "Calling esp_video_init...");
        esp_err_t err = esp_video_init(&cam_config);
        if (err != ESP_OK) {
            ESP_LOGE("WhoP4Cam", "Video Init Failed: %s (0x%x)", esp_err_to_name(err), err);
            return false;
        }
        ESP_LOGI("WhoP4Cam", "esp_video_init succeeded!");
        return true;
    }();
    
    if (!once) {
        ESP_LOGE(TAG, "Static initialization failed - aborting");
        abort();  // Make it clear this is a fatal error
    }
    ESP_LOGI(TAG, "Static initialization successful, proceeding...");

    ESP_ERROR_CHECK(open_video_device());
    ESP_ERROR_CHECK(print_info());
    ESP_ERROR_CHECK(set_flip(vertical_flip, horizontal_flip));
    ESP_ERROR_CHECK(set_video_format());
    ESP_ERROR_CHECK(init_fbs());
    ESP_ERROR_CHECK(start_video_stream());
}

void WhoP4Cam::video_deinit()
{
    ESP_ERROR_CHECK(stop_video_stream());
    ESP_ERROR_CHECK(close_video_device());
    ESP_ERROR_CHECK(esp_video_deinit());
}

esp_err_t WhoP4Cam::open_video_device()
{
    ESP_LOGI(TAG, "Opening device: %s", ESP_VIDEO_MIPI_CSI_DEVICE_NAME);
    
    // Device should exist now, try opening with minimal flags
    m_fd = open(ESP_VIDEO_MIPI_CSI_DEVICE_NAME, O_RDWR);
    
    if (m_fd < 0) {
        ESP_LOGE(TAG, "✗ Failed to open %s", ESP_VIDEO_MIPI_CSI_DEVICE_NAME);
        ESP_LOGE(TAG, "  Error: %s (errno=%d)", strerror(errno), errno);
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "✓ Device opened successfully (fd=%d)", m_fd);
    return ESP_OK;
}

esp_err_t WhoP4Cam::print_info()
{
    struct v4l2_capability capability;
    if (ioctl(m_fd, VIDIOC_QUERYCAP, &capability)) {
        ESP_LOGE(TAG, "Failed to get capability");
        close(m_fd);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG,
             "version: %d.%d.%d",
             (uint16_t)(capability.version >> 16),
             (uint8_t)(capability.version >> 8),
             (uint8_t)capability.version);
    ESP_LOGI(TAG, "driver:  %s", capability.driver);
    ESP_LOGI(TAG, "card:    %s", capability.card);
    ESP_LOGI(TAG, "bus:     %s", capability.bus_info);
    ESP_LOGI(TAG, "capabilities:");
    if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        ESP_LOGI(TAG, "\tVIDEO_CAPTURE");
    }
    if (capability.capabilities & V4L2_CAP_READWRITE) {
        ESP_LOGI(TAG, "\tREADWRITE");
    }
    if (capability.capabilities & V4L2_CAP_ASYNCIO) {
        ESP_LOGI(TAG, "\tASYNCIO");
    }
    if (capability.capabilities & V4L2_CAP_STREAMING) {
        ESP_LOGI(TAG, "\tSTREAMING");
    }
    if (capability.capabilities & V4L2_CAP_META_OUTPUT) {
        ESP_LOGI(TAG, "\tMETA_OUTPUT");
    }
    if (capability.capabilities & V4L2_CAP_DEVICE_CAPS) {
        ESP_LOGI(TAG, "device capabilities:");
        if (capability.device_caps & V4L2_CAP_VIDEO_CAPTURE) {
            ESP_LOGI(TAG, "\tVIDEO_CAPTURE");
        }
        if (capability.device_caps & V4L2_CAP_READWRITE) {
            ESP_LOGI(TAG, "\tREADWRITE");
        }
        if (capability.device_caps & V4L2_CAP_ASYNCIO) {
            ESP_LOGI(TAG, "\tASYNCIO");
        }
        if (capability.device_caps & V4L2_CAP_STREAMING) {
            ESP_LOGI(TAG, "\tSTREAMING");
        }
        if (capability.device_caps & V4L2_CAP_META_OUTPUT) {
            ESP_LOGI(TAG, "\tMETA_OUTPUT");
        }
    }
    return ESP_OK;
}

esp_err_t WhoP4Cam::close_video_device()
{
    close(m_fd);
    return ESP_OK;
}

esp_err_t WhoP4Cam::set_video_format()
{
    if (CONFIG_CAMERA_SC2336_MIPI_IF_FORMAT_INDEX_DEFAULT < 8) {
        ESP_LOGE(TAG, "raw10 cam mode is not supported.");
        close(m_fd);
        return ESP_FAIL;
    }
    struct v4l2_format format = {};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_G_FMT, &format) != 0) {
        ESP_LOGE(TAG, "Failed to get format");
        close(m_fd);
        return ESP_FAIL;
    }
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = m_v4l2_fmt;
    m_fb_width = format.fmt.pix.width;
    m_fb_height = format.fmt.pix.height;
    if (ioctl(m_fd, VIDIOC_S_FMT, &format) != 0) {
        ESP_LOGE(TAG, "Failed to set format.");
        ESP_LOGI(TAG, "Supported format:");
        uint32_t idx = 0;
        while (true) {
            struct v4l2_fmtdesc fmtdesc = {};
            fmtdesc.index = idx++;
            fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (ioctl(m_fd, VIDIOC_ENUM_FMT, &fmtdesc) != 0) {
                break;
            }
            ESP_LOGI(TAG, "\t%s", (char *)fmtdesc.description);
        }
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t WhoP4Cam::init_fbs()
{
    struct v4l2_requestbuffers req = {};
    req.count = m_fb_count;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = m_fb_mem_type;
    if (ioctl(m_fd, VIDIOC_REQBUFS, &req) != 0) {
        ESP_LOGE(TAG, "Failed to require buffer");
        close(m_fd);
        return ESP_FAIL;
    }

    for (size_t i = 0; i < m_fb_count; i++) {
        struct v4l2_buffer *buf = new struct v4l2_buffer();
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = m_fb_mem_type;
        buf->index = i;

        if (ioctl(m_fd, VIDIOC_QUERYBUF, buf) != 0) {
            ESP_LOGE(TAG, "Failed to query buffer");
            close(m_fd);
            return ESP_FAIL;
        }

        cam_fb_t *fb = m_cam_fbs + i;
        if (m_fb_mem_type == V4L2_MEMORY_MMAP) {
            fb->buf = mmap(NULL, buf->length, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, buf->m.offset);
        } else {
            // implicit align to cache line size
            fb->buf = heap_caps_malloc(buf->length, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
            buf->m.userptr = (unsigned long)fb->buf;
        }
        if (!fb->buf) {
            ESP_LOGE(TAG, "Failed to map/alloc buffer");
            close(m_fd);
            return ESP_FAIL;
        }
        fb->width = m_fb_width;
        fb->height = m_fb_height;
        fb->format = v4l2_fmt2cam_fb_fmt(m_v4l2_fmt);
        fb->ret = (void *)buf;

        if (ioctl(m_fd, VIDIOC_QBUF, buf) != 0) {
            ESP_LOGE(TAG, "Failed to queue video frame");
            close(m_fd);
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t WhoP4Cam::start_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_STREAMON, &type) != 0) {
        ESP_LOGE(TAG, "Failed to start stream");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t WhoP4Cam::stop_video_stream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(m_fd, VIDIOC_STREAMOFF, &type) != 0) {
        ESP_LOGE(TAG, "Failed to stop stream");
        close(m_fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}

cam_fb_t *WhoP4Cam::cam_fb_get()
{
    struct v4l2_buffer buf = {};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = m_fb_mem_type;
    if (ioctl(m_fd, VIDIOC_DQBUF, &buf) != 0) {
        ESP_LOGE(TAG, "failed to receive video frame");
        return nullptr;
    }
    int64_t us = esp_timer_get_time();
    cam_fb_t *fb = m_cam_fbs + buf.index;
    fb->timestamp.tv_sec = us / 1000000;
    fb->timestamp.tv_usec = us % 1000000;
    fb->len = buf.bytesused;
    return fb;
}

void WhoP4Cam::cam_fb_return(cam_fb_t *fb)
{
    if (ioctl(m_fd, VIDIOC_QBUF, fb->ret) != 0) {
        ESP_LOGE(TAG, "failed to queue video frame");
    }
}

} // namespace cam
} // namespace who

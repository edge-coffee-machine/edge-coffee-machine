// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial
#include "platform_config.h"

#ifdef QUL_PLATFORM_DEVICELINK_ENABLED
#include "platform_display.h"

#include <platform/devicelink.h>
#include <platforminterface/devicelink.h>
#include <platforminterface/log.h>
#include <platforminterface/platforminterface.h>
#include <platforminterface/screen.h>

#include <esp_log.h>
#include <driver/usb_serial_jtag.h>
#include <driver/usb_serial_jtag_vfs.h>
#include <hal/usb_serial_jtag_ll.h>
#include <soc/interrupts.h>
#include <soc/periph_defs.h>

#include <iostream>

extern "C" {
static void usbSerialJtagIsrHandler(void *arg)
{
    uint32_t usbjtag_intr_status = usb_serial_jtag_ll_get_intsts_mask();
    usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
    if (usbjtag_intr_status & USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT) {
        uint8_t rx_data_buf[16];
        while (1) {
            uint32_t rx_fifo_len = usb_serial_jtag_ll_read_rxfifo(rx_data_buf, sizeof(rx_data_buf));
            if (rx_fifo_len == 0) {
                break;
            }
            Qul::PlatformInterface::deviceLinkBytesReceived(rx_data_buf, rx_fifo_len);
            /* Wake-up the task for processing incoming commands. */
            portYIELD_FROM_ISR();
        }
    }
}
}

struct Esp32DeviceLinkInterface : Qul::Platform::DeviceLinkInterface
{
    static int custom_vprintf(const char *format, va_list arg)
    {
        char buf[81];
        int ret = vsnprintf(buf, sizeof(buf), format, arg);
        if (ret > 0) {
            auto deviceLink = Qul::Platform::DeviceLink::instance();
            if (deviceLink) {
                int count = ret >= sizeof(buf) ? (sizeof(buf) - 1) : ret;
                deviceLink->printMessage(buf, count);
            }
        }
        return ret;
    }

    void platformInit()
    {
        esp_err_t err = ESP_OK;
        usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
        usb_serial_jtag_ll_ena_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
        err = esp_intr_alloc(ETS_USB_SERIAL_JTAG_INTR_SOURCE, 0, usbSerialJtagIsrHandler, NULL, NULL);
        if (err != ESP_OK) {
            Qul::PlatformInterface::log("DeviceLink: INT alloc failed!\n");
        }
    }

    void configureSerialForDevicelink()
    {
        /* Grab log messages for re-directing to DeviceLink. This might not be optimal
         * solution, but at least it grabs messages from ESP-IDF's logging system,
         * wrap them into DeviceLink frames and prevents logs messing up with the protocol.
        */
        esp_log_set_vprintf(custom_vprintf);

        /* ESP-IDF's USB serial JTAG driver handles written data as text and likes
         * to alter newline control characters. Make sure data is kept as is by
         * forcing the newline mode use <LF> line ending which does not cause any
         * changes to sent data.
         */
        usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_LF);

        /* Make sure stdout is not buffered, so all protocol data gets out properly. */
        setvbuf(stdout, NULL, _IONBF, 0);
    }

    void transmitChars(const uint8_t *data, uint32_t size)
    {
        static bool serialConfiguredForDevicelink = false;
        if (!serialConfiguredForDevicelink) {
            configureSerialForDevicelink();
            serialConfiguredForDevicelink = true;
        }

        for (int i = 0; i < size; i++) {
            putchar(*data++);
        }
        /* Make sure outgoing bytes are flushed from TX buffer. */
        fsync(STDOUT_FILENO);
    }

    Qul::Platform::FramebufferFormat framebufferFormat(const Qul::PlatformInterface::LayerEngine::ItemLayer *layer)
        QUL_DECL_OVERRIDE
    {
        Qul::Platform::FramebufferFormat format;

        format.address = Qul::Platform::Private::lcdFrameBuffer();
        format.width = Qul::Platform::Private::lcdWidth();
        format.height = Qul::Platform::Private::lcdHeight();
        format.bytesPerLine = Qul::Platform::Private::lcdWidth() * Qul::Platform::Private::lcdBytesPerPixel();
        format.bitsPerPixel = Qul::Platform::Private::lcdBytesPerPixel() * 8;
        format.redChannel.offset = 0;
        format.redChannel.length = 5;
        format.greenChannel.offset = 5;
        format.greenChannel.length = 6;
        format.blueChannel.offset = 11;
        format.blueChannel.length = 5;
        format.swapBytes = 2;
        return format;
    }
};
#endif // QUL_PLATFORM_DEVICELINK_ENABLED

namespace Qul {
namespace Platform {

#ifdef QUL_PLATFORM_DEVICELINK_ENABLED
DeviceLinkInterface *getDeviceLinkInterface()
{
    static Esp32DeviceLinkInterface deviceLink;
    return &deviceLink;
}
#endif

} // namespace Platform
} // namespace Qul

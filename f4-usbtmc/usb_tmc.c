
#include <stdlib.h>
#include <libopencm3/usb/usbstd.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/usb/usbd.h>
#include <string.h>

#include "syscfg.h"
#include "tmc.h"
#include "usb_tmc.h"

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];
usbd_device *tmc_dev;

static const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x2000,
	.bDeviceClass = 0, // on interface
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0xc03e,
	.idProduct = 0xb007,
	.bcdDevice = 0x2000,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};


static const struct usb_endpoint_descriptor data_endp[] = {
	{
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = 0x01,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = 64,
		.bInterval = 1,
	},
	{
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = 0x82,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = 64,
		.bInterval = 1,
	}};


static const struct usb_interface_descriptor tmc_iface[] = {
	{
		.bLength = USB_DT_INTERFACE_SIZE,
		.bDescriptorType = USB_DT_INTERFACE,
		.bInterfaceNumber = 0,
		.bAlternateSetting = 0,
		.bNumEndpoints = 2,
		.bInterfaceClass = USB_CLASS_APPLICATION,
		.bInterfaceSubClass = USB_APPLICATION_SUBCLASS_TMC,
		.bInterfaceProtocol = USB_TMC_PROTOCOL_NONE,
		.iInterface = 0,

		.endpoint = data_endp,
	}};


static const struct usb_interface ifaces[] = {
	{
		.num_altsetting = 1,
		.altsetting = tmc_iface,
	}};

static const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0x80,
	.bMaxPower = 0x32,

	.interface = ifaces,
};

static const char *usb_strings[] = {
	"libopencm3",
	"usbtmc sample",
	"none",
	"DEMO",
};

static const struct usb_tmc_get_capabilities_response capabilities = {
	.USBTMC_status = USB_TMC_STATUS_SUCCESS,
	.bcdUSBTMC = 0x0100,
	.reserved0 = 0,
	.reserved1 = {0},
	.reserved_subclass = {0},
	.interface_capabilities = USB_TMC_INTERFACE_CAPABILITY_INDICATOR_PULSE,
	.device_capabilities = 0
};

void usb_tmc_setup_pre_arch(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_OTGFS);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO9 | GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);

}

void usb_tmc_setup_post_arch(void)
{
	/* Better enable interrupts */
	nvic_enable_irq(NVIC_OTG_FS_IRQ);
}

/*
 * You're meant to turn it on for a "humanly visible period" then autooff
 * recommended is on for > 500ms and < 1 sec.
 * We're hacking it a bit now with just a toggle each time you ask for it
 */
void usb_tmc_arch_handle_pulse(void) {
	gpio_toggle(LED_PULSE_PORT, LED_PULSE_PIN);
}

/**
 * Don't believe I have any to do right now?
 * need to handle the setup packets? section 4.2.1?
 * @param usbd_dev
 * @param req
 * @param buf
 * @param len
 * @param complete
 * @return 
 */
static
int tmc_control_request(usbd_device *usbd_dev,
	struct usb_setup_data *req,
	uint8_t **buf,
	uint16_t *len,
	void (**complete) (usbd_device *usbd_dev, struct usb_setup_data *req))
{

	(void) complete;
	(void) buf;
	(void) usbd_dev;

	switch (req->bRequest) {
		
	case USB_REQ_CLEAR_FEATURE:
		if (req->wIndex != 0) {
			if (req->wValue == USB_ENDPOINT_HALT) {
				// USBTMC says we're meant to do some _extra_ handling of state here?
				// but let the existing code handle the stall stuff
				return USBD_REQ_NEXT_CALLBACK;
			}
		}
		//

	case USB_TMC_REQ_GET_CAPABILITIES:
		memcpy(*buf, &capabilities, sizeof(capabilities));
		return USBD_REQ_HANDLED;
	case USB_TMC_REQ_INDICATOR_PULSE:
		usb_tmc_arch_handle_pulse();
		return USBD_REQ_HANDLED;
	}
	return 0;
}

static void tmc_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	uint8_t buf[64];

	(void) ep;

	int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);
	//glue_send_data_cb(buf, len);
}

static void tmc_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void) wValue;

	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64,
		tmc_data_rx_cb);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);

	usbd_register_control_callback(usbd_dev,
		USB_REQ_TYPE_CLASS |
		USB_REQ_TYPE_INTERFACE,
		USB_REQ_TYPE_TYPE |
		USB_REQ_TYPE_RECIPIENT,
		tmc_control_request);
}

void usb_tmc_init(usbd_device **usbd_dev)
{
	usb_tmc_setup_pre_arch();

	// 4 == ARRAY_LENGTH(usb_strings)
	*usbd_dev = usbd_init(&otgfs_usb_driver, &dev, &config, usb_strings, 4,
		usbd_control_buffer, sizeof (usbd_control_buffer));
	tmc_dev = *usbd_dev;
	usbd_register_set_config_callback(tmc_dev, tmc_set_config);

	usb_tmc_setup_post_arch();
}

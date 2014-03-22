/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2014 Karl Palsson <karlp@tweak.net.au>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * XXXXX
 */

#ifndef USB_TMC_H
#define	USB_TMC_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/usb/usbd.h>

struct __attribute__((packed)) usb_tmc_get_capabilities_response
{
	uint8_t USBTMC_status;
	uint8_t reserved0;
	uint16_t bcdUSBTMC;
	uint8_t interface_capabilities; /* bitmap! */
	uint8_t device_capabilities; /* bitmap! */
	uint8_t reserved1[6];
	uint8_t reserved_subclass[12];
};

BEGIN_DECLS

void usb_tmc_init(usbd_device **usb_dev, const char *serial_number);
void usb_tmc_setup_pre_arch(void);
void usb_tmc_setup_post_arch(void);
void tmc_glue_send_data(uint8_t *buf, size_t len);

END_DECLS

#endif	/* USB_TMC_H */


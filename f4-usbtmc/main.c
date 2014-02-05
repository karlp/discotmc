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

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "syscfg.h"
#include "usb_tmc.h"
#include "scpi-arch.h"
#include "funcgen.h"

int _write(int file, char *ptr, int len);

static
void usart_init(int baud)
{
	rcc_periph_clock_enable(USART_CONSOLE_RCC);
	rcc_periph_clock_enable(USART_CONSOLE_PINS_RCC);
	rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPAEN);

	/* Setup GPIO pins for USART2 transmit. */
	gpio_mode_setup(USART_CONSOLE_PINS_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART_CONSOLE_PINS_PINS);
	gpio_set_af(USART_CONSOLE_PINS_PORT, GPIO_AF7, USART_CONSOLE_PINS_PINS);

	usart_set_baudrate(USART_CONSOLE, baud);
	usart_set_databits(USART_CONSOLE, 8);
	usart_set_stopbits(USART_CONSOLE, USART_STOPBITS_1);
	usart_set_mode(USART_CONSOLE, USART_MODE_TX);
	usart_set_parity(USART_CONSOLE, USART_PARITY_NONE);
	usart_set_flow_control(USART_CONSOLE, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART_CONSOLE);
}

/**
 * Use USART_CONSOLE as a console.
 * This is a syscall for newlib
 * @param file
 * @param ptr
 * @param len
 * @return
 */
int _write(int file, char *ptr, int len)
{
	int i;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				usart_send_blocking(USART_CONSOLE, '\r');
			}
			usart_send_blocking(USART_CONSOLE, ptr[i]);
		}
		return i;
	}
	errno = EIO;
	return -1;
}

int main(void)
{
	usbd_device *usbd_dev;

	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);
	/* Leds are on port D */
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(LED_RX_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_RX_PIN | LED_TX_PIN | LED_PULSE_PIN);

	usart_init(115200);
	usb_tmc_init(&usbd_dev);
	scpi_init();
	funcgen_init_arch();
	printf("Disco non stop TMC!\n");

	while (1) {
		//__WFI();
	}

}


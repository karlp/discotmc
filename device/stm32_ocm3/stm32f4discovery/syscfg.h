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

#ifndef SYSCFG_H
#define	SYSCFG_H

#ifdef	__cplusplus
extern "C" {
#endif

/* TODO: should really make a stm32f4discovery.h file... */

#define LED_RX_PORT	GPIOD
#define LED_RX_PIN	GPIO15  /* Blue, but you won't see this one much */
#define LED_TX_PORT	GPIOD
#define LED_TX_PIN	GPIO13  /* orange */
#define LED_PULSE_PORT	GPIOD
#define LED_PULSE_PIN	GPIO14  /* red */

#define USART_CONSOLE USART2
#define USART_CONSOLE_RCC	RCC_USART2
#define USART_CONSOLE_PINS_RCC RCC_GPIOA
#define USART_CONSOLE_PINS_PORT GPIOA
#define USART_CONSOLE_PINS_PINS (GPIO2 | GPIO3)

#define TIMER_DAC1_ISR		tim6_dac_isr
#define TIMER_DAC1_NVIC         NVIC_TIM6_DAC_IRQ

#define USB_ISR		otg_fs_isr
#define USB_ISR_NVIC	NVIC_OTG_FS_IRQ
#define USB_RCC         RCC_OTGFS
#define USB_DRIVER	otgfs_usb_driver

void clock_arch_setup(void);



#ifdef	__cplusplus
}
#endif

#endif	/* SYSCFG_H */


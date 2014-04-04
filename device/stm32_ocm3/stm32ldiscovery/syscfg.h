/*
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


/* only two leds on 32l board;) */
#define LED_RX_PORT	GPIOB
#define LED_RX_PIN	GPIO7 /* green */
#define LED_TX_PORT	GPIOB
#define LED_TX_PIN	GPIO7
#define LED_PULSE_PORT	GPIOB
#define LED_PULSE_PIN	GPIO6 /* blue */

#define USART_CONSOLE USART2
#define USART_CONSOLE_RCC	RCC_USART2
#define USART_CONSOLE_PINS_RCC RCC_GPIOA
#define USART_CONSOLE_PINS_PORT GPIOA
#define USART_CONSOLE_PINS_PINS (GPIO2 | GPIO3)

#define TIMER_DAC1_ISR		tim6_isr
#define TIMER_DAC1_NVIC		NVIC_TIM6_IRQ

#define USB_ISR         usb_lp_isr
#define USB_ISR_NVIC    NVIC_USB_LP_IRQ
#define USB_RCC		RCC_USB /* hoho */
#define USB_DRIVER      stm32f103_usb_driver




	/* Hope everyone used the same defn ;) */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

BEGIN_DECLS

void clock_arch_setup(void);

END_DECLS

#endif	/* SYSCFG_H */


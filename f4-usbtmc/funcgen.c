#include <stdint.h>
#include <stdio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "funcgen.h"

static struct funcgen_state_t state;

/* From ST's example code */
const uint16_t lut_sine[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

// TODO - need to extract common ch1/ch2 code...
static void dma_setup_1(const uint16_t *wave_table, int wave_table_count)
{
	/* DAC channel 1 uses DMA controller 1 Stream 5 Channel 7. */
#define WANT_USE_DMA_INTERRUPTS 0
#if WANT_USE_DMA_INTERRUPTS
	nvic_enable_irq(NVIC_DMA1_STREAM5_IRQ);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_STREAM5);
#endif
	dma_stream_reset(DMA1, DMA_STREAM5);
	dma_set_priority(DMA1, DMA_STREAM5, DMA_SxCR_PL_LOW);
	dma_set_memory_size(DMA1, DMA_STREAM5, DMA_SxCR_MSIZE_16BIT);
	dma_set_peripheral_size(DMA1, DMA_STREAM5, DMA_SxCR_PSIZE_16BIT);
	dma_enable_memory_increment_mode(DMA1, DMA_STREAM5);
	dma_enable_circular_mode(DMA1, DMA_STREAM5);
	dma_set_transfer_mode(DMA1, DMA_STREAM5, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
	/* TODO - _can_ use DAC_DHR12RD if we've got both channels active! */
	dma_set_peripheral_address(DMA1, DMA_STREAM5, (uint32_t) &DAC_DHR12R1);
	dma_set_memory_address(DMA1, DMA_STREAM5, (uint32_t) wave_table);
	dma_set_number_of_data(DMA1, DMA_STREAM5, wave_table_count);
	dma_channel_select(DMA1, DMA_STREAM5, DMA_SxCR_CHSEL_7);
	dma_enable_stream(DMA1, DMA_STREAM5);
}

static void dac_setup_1(void)
{
	/* Setup the DAC channel 1, with timer 2 as trigger source.
	 * Assume the DAC has woken up by the time the first transfer occurs */
	dac_trigger_enable(CHANNEL_1);
	dac_set_trigger_source(DAC_CR_TSEL1_T6);
	dac_dma_enable(CHANNEL_1);
	dac_enable(CHANNEL_1);
}


static void timer_setup_1(void)
{
	timer_reset(TIM6);
	// 42Mhz / 100khz -1.  // 42 is apb1 max speed
	timer_set_prescaler(TIM6, 419);
	// 100khz for 10 ticks = 10 khz overflow = 100us overflow interrupts
	// == output frequency == wavelength * 100usec
	// == ~32 * 100usec == ~312Hz?
	timer_set_period(TIM6, 10);

	timer_set_master_mode(TIM6, TIM_CR2_MMS_UPDATE);
	timer_enable_counter(TIM6);
}





void funcgen_init_arch(void) {
	rcc_periph_clock_enable(RCC_DMA1);
	rcc_periph_clock_enable(RCC_DAC);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_TIM6); /* channel 1 triggers */
	rcc_periph_clock_enable(RCC_TIM7); /* channel 2 triggers */
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4|GPIO5);
}

/* gross api! */
void funcgen_go(void) {
	timer_setup_1();
	dma_setup_1(lut_sine, ARRAY_LENGTH(lut_sine));
	dac_setup_1();
	state.outputs[0] = true;
}

void funcgen_stop(void) {
	dac_disable(CHANNEL_1);
	state.outputs[0] = false;
}

struct funcgen_state_t* funcgen_getstate(void) {
	return &state;
}
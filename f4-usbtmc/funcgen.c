#include <stdint.h>
#include <stdio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "funcgen.h"

static struct funcgen_output_t _output1 = {
	.enabled = false,
	.ampl = 0.1,
	.offset = FULL_SCALE / 2,
	.freq = 1000,
};

static struct funcgen_output_t _output2 = {
	.enabled = false,
	.ampl = 0.1,
	.offset = FULL_SCALE / 2,
	.freq = 1000,
};

static struct funcgen_state_t state = {
	.outputs = { &_output1, &_output2 },
};

/* http://yehar.com/blog/?p=1220 perhaps */
/* From ST's example code */
const uint16_t lut_sine[] = {
	2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
	3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
	599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647
};

// TODO - need to extract common ch1/ch2 code...

/* FIXME - move this to a layer of platform code, with "null", "f4" and "l1" platforms */
static void dma_setup(int channel, const uint16_t *wave_table, int wave_table_count) {
	/* DAC channel 1 uses DMA controller 1 Stream 5 Channel 7. */
	/* DAC channel 2 uses DMA controller 1 Stream 6 Channel 7. */
	int stream;
	switch (channel) {
	case CHANNEL_2:
		stream = DMA_STREAM6;
		break;
	default:
	case CHANNEL_1:
		stream = DMA_STREAM5;
		break;
	}
#define WANT_USE_DMA_INTERRUPTS 0
#if WANT_USE_DMA_INTERRUPTS
	nvic_enable_irq(NVIC_DMA1_STREAM5_IRQ);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_STREAM5);
#endif
	dma_stream_reset(DMA1, stream);
	dma_set_priority(DMA1, stream, DMA_SxCR_PL_LOW);
	dma_set_memory_size(DMA1, stream, DMA_SxCR_MSIZE_16BIT);
	dma_set_peripheral_size(DMA1, stream, DMA_SxCR_PSIZE_16BIT);
	dma_enable_memory_increment_mode(DMA1, stream);
	dma_enable_circular_mode(DMA1, stream);
	dma_set_transfer_mode(DMA1, stream, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
	/* TODO - _can_ use DAC_DHR12RD if we've got both channels active! */
	dma_set_peripheral_address(DMA1, stream, (uint32_t) & DAC_DHR12R1);
	dma_set_memory_address(DMA1, stream, (uint32_t) wave_table);
	dma_set_number_of_data(DMA1, stream, wave_table_count);
	/* DAC is channel 7 for both dacs */
	dma_channel_select(DMA1, stream, DMA_SxCR_CHSEL_7);
	dma_enable_stream(DMA1, stream);
}

static void dac_setup(int channel) {
	/* Setup the DAC channel 1, with timer 2 as trigger source.
	 * Assume the DAC has woken up by the time the first transfer occurs */
	int chan;
	switch (channel) {
	case 1: dac_set_trigger_source(DAC_CR_TSEL2_T7);
		chan = CHANNEL_2;
		break;
	default:
	case 0:
		dac_set_trigger_source(DAC_CR_TSEL1_T6);
		chan = CHANNEL_1;
		break;
	}
	dac_trigger_enable(chan);
	dac_dma_enable(chan);
	dac_enable(chan);
}

static void timer_setup(int channel, int period_us) {
	uint32_t timer;
	switch (channel) {
	case 1: timer = TIM7;
		break;
	case 0:
	default:
		timer = TIM6;
		break;
	}
	timer_reset(timer);
	// APB is maxed at 42Mhz, so APB timers run at 84Mhz
	// dac says 1msps max max, so at 1msps, we want a period of what, 1 Mhz _overflows_
	// so at least 2 Mhz clock..., let's say 4 Mhz timer clock for max res stuff
	if (period_us < 4000) {
		timer_set_prescaler(timer, 20);  // 84MHz / 4MHz - 1 => ticks at 0.25usec
		timer_set_period(timer, (period_us * 4) - 1);
	} else {
		printf("out of range period request: %d", period_us);
	}

	timer_enable_irq(timer, TIM_DIER_UIE);

	timer_set_master_mode(timer, TIM_CR2_MMS_UPDATE);
	timer_enable_counter(timer);
}

void tim6_dac_isr(void) {
	timer_clear_flag(TIM6, TIM_SR_UIF);
	gpio_toggle(GPIOC, GPIO1);
}

void funcgen_init_arch(void) {
	rcc_periph_clock_enable(RCC_DMA1);
	rcc_periph_clock_enable(RCC_DAC);
	rcc_periph_clock_enable(RCC_GPIOA); /* DAC output pins */
	rcc_periph_clock_enable(RCC_TIM6); /* channel 1 triggers */
	rcc_periph_clock_enable(RCC_TIM7); /* channel 2 triggers */
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4 | GPIO5);

	/* timing tests */
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);

	/* Just enable both permanently in NVIC */
	nvic_enable_irq(NVIC_TIM6_DAC_IRQ); /* only used for timing debug */
	/* nvic_enable_irq(NVIC_TIM7_IRQ); */
	
	/* hack to auto start */
	//funcgen_sin(0, 1000, FULL_SCALE / 2, FULL_SCALE / 2);
}

/*
 * I ARE DUM
 * http://stackoverflow.com/questions/3304513/stretching-out-an-array
 */
static float interp1(float x, const uint16_t a[], int n)
{
	if (x <= 0) {
		return a[0];
	}
	if (x >= n - 1) {
		return a[n - 1];
	}
	int j = x;
	return a[j] + (x - j) * (a[j + 1] - a[j]);
}

void calculate_output(const uint16_t *source, int source_len,
	uint16_t *dest, int dest_len,
	float ampl, float offset)
{
	float step = (source_len - 1) / (dest_len - 1) * 1.0;
	for (int i = 0; i < dest_len; i++) {
		float si = interp1(i*step, source, source_len);
		float offset_bits = offset / FULL_SCALE * 4095;
		dest[i] = si * ampl / FULL_SCALE + offset_bits;
	}
}

void funcgen_sin(int channel, float frequency, float ampl, float offset) {
	uint16_t *wavedata = state.outputs[channel]->waveform;
	
	/* Take the input wave and calculate the wavetable for DMA */
	calculate_output(lut_sine, ARRAY_LENGTH(lut_sine), wavedata, ARRAY_LENGTH(wavedata),
		ampl, offset);

	int usecs_per_wave = 1000000 / frequency;
	int sample_period_us = usecs_per_wave / ARRAY_LENGTH(wavedata);
	printf("Requested freq: %f, usecs/wave: %d, timerusec: %d\n", frequency, usecs_per_wave, sample_period_us);
	
	/*+++ hardware setup +++*/
	timer_setup(channel, sample_period_us);
	dma_setup(channel, wavedata, ARRAY_LENGTH(wavedata));
	dac_setup(channel);
	/*++++++++++++++++++++++*/
	
	state.outputs[channel]->enabled = true;
	state.outputs[channel]->freq = frequency;
	state.outputs[channel]->ampl = ampl;
	state.outputs[channel]->offset = offset;
}

void funcgen_output(int channel, bool enable)
{
	switch (channel) {
	case 1: if (enable) {
			dac_enable(CHANNEL_2);
		} else {
			dac_disable(CHANNEL_2);
		}
		break;
	case 0:
	default:
		if (enable) {
			dac_enable(CHANNEL_1);
		} else {
			dac_disable(CHANNEL_1);
		}
		break;
	}
	state.outputs[channel]->enabled = enable;
}

struct funcgen_state_t* funcgen_getstate(void) {
	return &state;
}
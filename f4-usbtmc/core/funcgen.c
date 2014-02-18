#include <stdint.h>
#include <stdio.h>

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

static void calculate_output(const uint16_t *source, int source_len,
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
	funcgen_plat_timer_setup(channel, sample_period_us);
	funcgen_plat_dma_setup(channel, wavedata, ARRAY_LENGTH(wavedata));
	funcgen_plat_dac_setup(channel);
	/*++++++++++++++++++++++*/
	
	state.outputs[channel]->enabled = true;
	state.outputs[channel]->freq = frequency;
	state.outputs[channel]->ampl = ampl;
	state.outputs[channel]->offset = offset;
}

void funcgen_output(int channel, bool enable)
{
	funcgen_plat_output(channel, enable);
	state.outputs[channel]->enabled = enable;
}

struct funcgen_state_t* funcgen_getstate(void) {
	return &state;
}
/* 
 * File:   funcgen.h
 * Author: karlp
 *
 * Created on February 5, 2014, 8:09 PM
 */

#ifndef FUNCGEN_H
#define	FUNCGEN_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif
	/* TODO - put this somewhere more useful */
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))
#define FUNCGEN_WAVE_SIZE	512
#define FULL_SCALE 3.0

	enum _funcgen_output_mode {
		OUTPUT_MODE_SINE,
		OUTPUT_MODE_TRIANGLE,
		OUTPUT_MODE_SQUARE,
		OUTPUT_MODE_USER,
		OUTPUT_MODE_NULL,
	};

	struct _output_mode {
		const char * name;
		enum _funcgen_output_mode mode;
	};

	struct funcgen_output_t {
		bool enabled;
		/* In volts, p2p */
		float ampl;
		/* In volts, only sane from 0-FULL_SCALE*/
		float offset;
		/* number of counts of "waveform" to play per second */
		float freq;
		enum _funcgen_output_mode mode;
		/* Output buffers after calculating ampl and offset, or user */
		uint16_t waveform[FUNCGEN_WAVE_SIZE];
		/* What portion of the waveform to consider valid */
		int waveform_length;
	};

	struct funcgen_state_t {
		struct funcgen_output_t *outputs[2];
	};

	/* gross api! */
	void funcgen_sin(int channel, float frequency, float ampl, float offset);
	void funcgen_square(int channel, float frequency, float ampl, float offset);
	void funcgen_triangle(int channel, float frequency, float ampl, float offset);
	void funcgen_output(int channel, bool enable);
	struct funcgen_state_t * funcgen_getstate(void);

	/* platform must provide these routines */
	void funcgen_plat_init(void);
	void funcgen_plat_timer_setup(int channel, int period_us);
	void funcgen_plat_dac_setup(int channel);
	void funcgen_plat_dma_setup(int channel, const uint16_t *wave_table, int wave_table_count);
	void funcgen_plat_output(int channel, bool enable);

#ifdef	__cplusplus
}
#endif

#endif	/* FUNCGEN_H */


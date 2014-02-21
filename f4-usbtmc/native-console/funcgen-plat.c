#include <stdio.h>

#include "../core/funcgen.h"

void funcgen_plat_timer_setup(int channel, int period_us)
{
	printf("FUNCGEN:NULL:timer_setup channel: %d, period_us: %d\n", channel, period_us);
}

void funcgen_plat_dma_setup(int channel, const uint16_t *wave_table, int wave_table_count)
{
	(void)wave_table;
	printf("FUNCGEN:NULL:dma_setup channel: %d, wavetable size: %d\n", channel, wave_table_count);
	// TODO - do something here to examine the data!
}

void funcgen_plat_dac_setup(int channel)
{
	printf("FUNCGEN:NULL:dac_setup channel: %d\n", channel);
}

void funcgen_plat_output(int channel, bool enable)
{
	printf("FUNCGEN:NULL:output channel: %d %s\n", channel, enable ? "on" : "off");
}

void funcgen_plat_init(void)
{
	/* nothign to do */
}



#include <stdint.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/dma.h>

#include "funcgen.h"


/* DMA setup is specific to L1, or, "not f4" at least */
void funcgen_plat_dma_setup(int channel, const uint16_t *wave_table, int wave_table_count) {
	/* DAC channel 1 uses DMA controller 1 Stream 5 Channel 7. */
	/* DAC channel 2 uses DMA controller 1 Stream 6 Channel 7. */
	int dma_channel;
	uint32_t daddr;
	switch (channel) {
	case CHANNEL_2:
		daddr = (uint32_t) & DAC_DHR12R2;
		dma_channel = DMA_CHANNEL3;
		break;
	default:
	case CHANNEL_1:
		daddr = (uint32_t) & DAC_DHR12R1;
		dma_channel = DMA_CHANNEL2;
		break;
	}

        dma_channel_reset(DMA1, dma_channel);

        dma_set_memory_size(DMA1, dma_channel, DMA_CCR_MSIZE_16BIT);
        dma_set_peripheral_size(DMA1, dma_channel, DMA_CCR_PSIZE_16BIT);

        dma_set_memory_address(DMA1, dma_channel, (uint32_t) wave_table);
        dma_set_peripheral_address(DMA1, dma_channel, daddr);

        dma_enable_memory_increment_mode(DMA1, dma_channel);
        dma_enable_circular_mode(DMA1, dma_channel);
        dma_set_number_of_data(DMA1, dma_channel, wave_table_count);
        dma_set_read_from_memory(DMA1, dma_channel);

        dma_enable_channel(DMA1, dma_channel);
}


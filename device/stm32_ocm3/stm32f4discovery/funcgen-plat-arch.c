
#include <stdint.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/dma.h>

#include "funcgen.h"


/* FIXME - move this to a layer of platform code, with "null", "f4" and "l1" platforms */
void funcgen_plat_dma_setup(int channel, const uint16_t *wave_table, int wave_table_count) {
	/* DAC channel 1 uses DMA controller 1 Stream 5 Channel 7. */
	/* DAC channel 2 uses DMA controller 1 Stream 6 Channel 7. */
	int stream;
	uint32_t daddr;
	switch (channel) {
	case CHANNEL_2:
		stream = DMA_STREAM6;
		daddr = (uint32_t) & DAC_DHR12R2;
		break;
	default:
	case CHANNEL_1:
		daddr = (uint32_t) & DAC_DHR12R1;
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
	dma_set_peripheral_address(DMA1, stream, daddr);
	dma_set_memory_address(DMA1, stream, (uint32_t) wave_table);
	dma_set_number_of_data(DMA1, stream, wave_table_count);
	/* DAC is channel 7 for both dacs */
	dma_channel_select(DMA1, stream, DMA_SxCR_CHSEL_7);
	dma_enable_stream(DMA1, stream);
}


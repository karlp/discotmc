#include <stdint.h>
#include <stdio.h>
#include "../core/dscpi.h"
#include "usb_tmc.h"

size_t scpi_impl_write(scpi_t *context, const char *data, size_t len)
{
	(void) context;
	//hexdump("scpi reply", (uint8_t *) data, len);
	tmc_glue_send_data((uint8_t *) data, len);
	return len;
}

int scpi_impl_error(scpi_t * context, int_fast16_t err)
{
	(void) context;
	printf("**ERROR: %d, \"%s\"\r\n", err, SCPI_ErrorTranslate(err));
	return 0;
}

scpi_result_t scpi_impl_reset(scpi_t *context)
{
	(void) context;
	/* TODO could do a full system reset here? */
	printf("Result handler got called!\n");
	return SCPI_RES_OK;
}

scpi_interface_t scpi_interface = {
	.write = scpi_impl_write,
	.error = scpi_impl_error,
	.reset = scpi_impl_reset,
	.test = NULL,
	.control = NULL,
};


void scpi_init_platform(char *serial_number) {
	/* gross having to pass this down twice.  must be a better way.... */
	dscpi_init(&scpi_interface, serial_number);
}
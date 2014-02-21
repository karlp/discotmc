
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "../core/dscpi.h"

size_t scpi_impl_write(scpi_t * context, const char * data, size_t len)
{
	if (context->user_context != NULL) {
		int fd = *(int *) (context->user_context);
		return write(fd, data, len);
	}
	return 0;
}

scpi_result_t scpi_impl_flush(scpi_t * context)
{
	(void) context;
	fprintf(stderr, "**Flush\n");
	return SCPI_RES_OK;
}

int scpi_impl_error(scpi_t * context, int_fast16_t err)
{
	(void) context;
	// BEEP
	fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int32_t) err, SCPI_ErrorTranslate(err));
	return 0;
}

scpi_result_t scpi_impl_reset(scpi_t * context)
{
	(void)context;
	fprintf(stderr, "**Reset\r\n");
	return SCPI_RES_OK;
}

scpi_result_t scpi_impl_control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val)
{
	(void)context;
	if (SCPI_CTRL_SRQ == ctrl) {
		fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
	} else {
		fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
	}
	return SCPI_RES_OK;
}

scpi_result_t scpi_impl_test(scpi_t * context)
{
	(void)context;
	fprintf(stderr, "**Test\r\n");
	return SCPI_RES_OK;
}

scpi_interface_t scpi_interface = {
	.write = scpi_impl_write,
	.error = scpi_impl_error,
	.reset = scpi_impl_reset,
	.test = scpi_impl_test,
	.control = scpi_impl_control,
	.flush = scpi_impl_flush,
};

void scpi_init_platform(char *serial_number)
{
	/* gross having to pass this down twice.  must be a better way.... */
	dscpi_init(&scpi_interface, serial_number);
}

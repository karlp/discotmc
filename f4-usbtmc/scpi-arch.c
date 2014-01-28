
#include <stdio.h>
#include <stdlib.h>
#include <scpi/scpi.h>
#include "scpi-arch.h"
#include "usb_tmc.h"

size_t myWrite(scpi_t * context, const char * data, size_t len);

scpi_command_t scpi_commands[] = {
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    { .pattern = "*RST", .callback = SCPI_CoreRst,},
    //{ .pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
    SCPI_CMD_LIST_END
};

scpi_interface_t scpi_interface = {
    .write = myWrite,
    .error = NULL,
    .reset = NULL,
    .test = NULL,
    .control = NULL,
};

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

//static scpi_reg_val_t scpi_regs[SCPI_REG_COUNT];

scpi_t scpi_context = {
    .cmdlist = scpi_commands,
    .buffer = {
        .length = SCPI_INPUT_BUFFER_LENGTH,
        .data = scpi_input_buffer,
    },
    .interface = &scpi_interface,
    //.registers = scpi_regs,
    .units = scpi_units_def,
    .special_numbers = scpi_special_numbers_def,
    .idn = {"Ekta labs", "DiscoTMC-F4", NULL, "0.1"},
};

void scpi_init(void) {	
	SCPI_Init(&scpi_context);
}

static void hexdump(char* prefix, uint8_t *buf, uint16_t len) {
	int i;
	printf("\n<%s: ", prefix);
	for (i = 0; i < len; i++) {
		printf("%x", buf[i]);
		if ((i + 1) % 16 == 0) {
			putchar('\n');
		} else {
			putchar(' ');
		}
	}
	putchar('>');
}

void scpi_glue_input(uint8_t *buf, uint16_t len, bool final)
{
	hexdump("feeding to scpi", buf, len);
	SCPI_Input(&scpi_context, (char *)buf, len);
	if (final) {
		SCPI_Input(&scpi_context, NULL, 0);
	}
}

size_t myWrite(scpi_t *context, const char *data, size_t len) {
	(void)context;
	hexdump("scpi reply", (uint8_t *)data, len);
	tmc_glue_send_data((uint8_t *)data, len);
	return len;
}


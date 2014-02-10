
#include <stdio.h>
#include <stdlib.h>
#include <scpi/scpi.h>
#include "scpi-arch.h"
#include "usb_tmc.h"
#include "funcgen.h"

size_t scpi_impl_write(scpi_t * context, const char * data, size_t len);
int scpi_impl_error(scpi_t * context, int_fast16_t err);
scpi_result_t scpi_imple_reset(scpi_t *context);
char numbuf[100]; // for printing numbers with units

/* SCPI HANDLERS ----------------------------- */

static int dscpi_output_inner(scpi_t *context, int output) {
	bool action;
	if (!SCPI_ParamBool(context, &action, true)) {
		return SCPI_RES_ERR;
	}
	printf("turning output %d %s", output + 1, action ? "on" : "off");
	if (action) {
		struct funcgen_state_t *fs;
		fs = funcgen_getstate();
		funcgen_go(output, fs->freq[output]);
	} else {
		funcgen_stop(output);
	}
	return SCPI_RES_OK;
}

scpi_result_t dscpi_output1(scpi_t *context) {
	return dscpi_output_inner(context, 0);
}

static int dscpi_outputQ_inner(scpi_t *context, int output) {
	printf("outputQ[%d]", output + 1);
	struct funcgen_state_t *fs;
	fs = funcgen_getstate();
	SCPI_ResultBool(context, fs->outputs[output]);
	return SCPI_RES_OK;
}

scpi_result_t dscpi_output1Q(scpi_t *context) {
	return dscpi_outputQ_inner(context, 0);
}

int dscpi_apply_sin_inner(scpi_t *context, int output) {
	scpi_number_t freq;
	scpi_number_t ampl;
	scpi_number_t offset;
	if (!SCPI_ParamNumber(context, &freq, false)) {
		/* you can only get here if you gave it a bad pointer */
	} else {
		if (freq.type == SCPI_NUM_DEF) {
			freq.value = 1000;
			freq.type = SCPI_NUM_NUMBER;
			freq.unit = SCPI_UNIT_HERTZ;
		}
		if (freq.unit == SCPI_UNIT_NONE) {
			freq.unit = SCPI_UNIT_HERTZ;
		}
		/* handle minimum, maximum, regular*/
	}
	if (!SCPI_ParamNumber(context, &ampl, false)) {
		/* bad pointers */
	} else {
		if (ampl.type == SCPI_NUM_DEF) {
			ampl.type = SCPI_NUM_NUMBER;
			ampl.unit = SCPI_UNIT_VOLT;
			ampl.value = 0.1; // 100 mV by default
		}
		if (ampl.unit == SCPI_UNIT_NONE) {
			ampl.unit = SCPI_UNIT_VOLT;
		}
		/* handle minimum, maximum, */
		/* TODO somehow work out how to handle Vrms units here ?*/
	}
	if (!SCPI_ParamNumber(context, &offset, false)) {
		/* bad pointers */
	} else {
		if (offset.type == SCPI_NUM_DEF) {
			offset.type = SCPI_NUM_NUMBER;
			offset.unit = SCPI_UNIT_VOLT;
			offset.value = 0;
		}
		if (offset.unit == SCPI_UNIT_NONE) {
			offset.unit = SCPI_UNIT_VOLT;
		}
		/* handle minimum, maximum */
	}
#define FULL_DEBUG 1
#if FULL_DEBUG
	SCPI_NumberToStr(context, &freq, numbuf, sizeof (numbuf));
	printf("setting output %d to sin wave freq: %s", output + 1, numbuf);
	SCPI_NumberToStr(context, &ampl, numbuf, sizeof (numbuf));
	printf("ampl: %s", numbuf);
	SCPI_NumberToStr(context, &offset, numbuf, sizeof (numbuf));
	printf("offset: %s", numbuf);
#endif
	funcgen_go(output, freq.value);
	return SCPI_RES_OK;
}

scpi_result_t dscpi_apply_sin1(scpi_t *context) {
	// Need to recalc shit here...
	return dscpi_apply_sin_inner(context, 0);
}

/* end scpi handlers ---------------------*/

scpi_command_t scpi_commands[] = {
	{ .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
	{ .pattern = "*RST", .callback = SCPI_CoreRst,},
	{ .pattern = "OUTP1", .callback = dscpi_output1,},
	{ .pattern = "OUTP1?", .callback = dscpi_output1Q,},
	{ .pattern = "SOUR1:APPLy:SIN", .callback = dscpi_apply_sin1,},
	SCPI_CMD_LIST_END
};

scpi_interface_t scpi_interface = {
	.write = scpi_impl_write,
	.error = scpi_impl_error,
	.reset = scpi_imple_reset,
	.test = NULL,
	.control = NULL,
};

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

//static scpi_reg_val_t scpi_regs[SCPI_REG_COUNT];

scpi_t scpi_context = {
	.cmdlist = scpi_commands,
	.buffer =
	{
		.length = SCPI_INPUT_BUFFER_LENGTH,
		.data = scpi_input_buffer,
	},
	.interface = &scpi_interface,
	//.registers = scpi_regs,
	.units = scpi_units_def,
	.special_numbers = scpi_special_numbers_def,
	.idn =
	{"Ekta labs", "DiscoTMC-F4", serial_number, "0.1"},
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

void scpi_glue_input(uint8_t *buf, uint16_t len, bool final) {
	hexdump("feeding to scpi", buf, len);
	SCPI_Input(&scpi_context, (char *) buf, len);
	if (final) {
		SCPI_Input(&scpi_context, NULL, 0);
	}
}

size_t scpi_impl_write(scpi_t *context, const char *data, size_t len) {
	(void) context;
	hexdump("scpi reply", (uint8_t *) data, len);
	tmc_glue_send_data((uint8_t *) data, len);
	return len;
}

int scpi_impl_error(scpi_t * context, int_fast16_t err) {
	(void) context;
	printf("**ERROR: %d, \"%s\"\r\n", err, SCPI_ErrorTranslate(err));
	return 0;
}

scpi_result_t scpi_imple_reset(scpi_t *context) {
	(void) context;
	/* TODO could do a full system reset here? */
	printf("Result handler got called!\n");
	return SCPI_RES_OK;
}
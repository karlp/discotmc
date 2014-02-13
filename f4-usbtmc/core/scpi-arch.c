/*
 * TODO - fix licenses
 * 
 * This is the _core_ scpi stuff
 */
#include <stdio.h>
#include <stdlib.h>
#include <scpi/scpi.h>
#include "scpi-arch.h"
#include "../stm32f4/usb_tmc.h"  // TODO extract serial numbe rout of this stuff
#include "funcgen.h"

static char numbuf[100]; // for printing numbers with units

/* handlers */
scpi_result_t dscpi_output1(scpi_t *context);
scpi_result_t dscpi_output1Q(scpi_t *context);
scpi_result_t dscpi_apply_sin1(scpi_t *context);
scpi_result_t dscpi_output2(scpi_t *context);
scpi_result_t dscpi_output2Q(scpi_t *context);
scpi_result_t dscpi_apply_sin2(scpi_t *context);
scpi_result_t dscpi_apply_sq1(scpi_t *context);
scpi_result_t dscpi_apply_noise1(scpi_t *context);
scpi_result_t dscpi_apply_user1(scpi_t *context);
scpi_result_t dscpi_apply1Q(scpi_t *context);
scpi_result_t dscpi_apply2Q(scpi_t *context);
scpi_result_t dscpi_data_dac1(scpi_t *context);
scpi_result_t dscpi_data_dac2(scpi_t *context);
scpi_result_t dscpi_data_dac1Q(scpi_t *context);
scpi_result_t dscpi_data_dac2Q(scpi_t *context);


/* commands */
scpi_command_t scpi_commands[] = {
	{ .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
	{ .pattern = "*RST", .callback = SCPI_CoreRst,},

	/* Required SCPI commands (SCPI std V1999.0 4.2.1) */
	{.pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
	{.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
	{.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},

	{ .pattern = "OUTP1", .callback = dscpi_output1,},
	{ .pattern = "OUTP2", .callback = dscpi_output2,},
	{ .pattern = "OUTP1?", .callback = dscpi_output1Q,},
	{ .pattern = "OUTP2?", .callback = dscpi_output2Q,},
	{ .pattern = "SOUR1:APPLy:SINe", .callback = dscpi_apply_sin1,},
	{ .pattern = "SOUR2:APPLy:SINe", .callback = dscpi_apply_sin2,},
	//		{ .pattern = "SOUR1:APPLy:SQUare", .callback = dscpi_apply_sq1,},
	//		{ .pattern = "SOUR1:APPLy:NOISe", .callback = dscpi_apply_noise1,},
	//		{ .pattern = "SOUR1:APPLy:USER", .callback = dscpi_apply_user1,},
	{ .pattern = "SOUR1:APPLy?", .callback = dscpi_apply1Q,},
	{ .pattern = "SOUR2:APPLy?", .callback = dscpi_apply2Q,},
	{ .pattern = "DATA1:DAC", .callback = dscpi_data_dac1,},
	{ .pattern = "DATA2:DAC", .callback = dscpi_data_dac2,},
	/* Why this isn't DATA[1|2]:DAC? I don't know...*/
	{ .pattern = "SOUR1:ARB:OUTPut", .callback = dscpi_data_dac1Q,},
	{ .pattern = "SOUR2:ARB:OUTPut", .callback = dscpi_data_dac2Q,},

	SCPI_CMD_LIST_END
};

static int dscpi_output_inner(scpi_t *context, int output)
{
	bool action;
	if (!SCPI_ParamBool(context, &action, true)) {
		return SCPI_RES_ERR;
	}
	printf("turning output %d %s", output + 1, action ? "on" : "off");
	funcgen_output(output, action);
	return SCPI_RES_OK;
}

scpi_result_t dscpi_output1(scpi_t *context)
{
	return dscpi_output_inner(context, 0);
}

scpi_result_t dscpi_output2(scpi_t *context)
{
	return dscpi_output_inner(context, 1);
}

static int dscpi_outputQ_inner(scpi_t *context, int output)
{
	printf("outputQ[%d]", output + 1);
	struct funcgen_state_t *fs;
	fs = funcgen_getstate();
	SCPI_ResultBool(context, fs->outputs[output]->enabled);
	return SCPI_RES_OK;
}

scpi_result_t dscpi_output1Q(scpi_t *context)
{
	return dscpi_outputQ_inner(context, 0);
}

scpi_result_t dscpi_output2Q(scpi_t *context)
{
	return dscpi_outputQ_inner(context, 1);
}

static int dscpi_apply_sin_inner(scpi_t *context, int output)
{
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
		if (ampl.type == SCPI_NUM_MIN) {
			ampl.value = 0;
		}
		if (ampl.type == SCPI_NUM_MAX) {
			ampl.value = FULL_SCALE;
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
			offset.value = FULL_SCALE / 2.0;
		}
		if (offset.type == SCPI_NUM_MIN) {
			offset.value = 0;
		}
		if (offset.type == SCPI_NUM_MAX) {
			offset.value = FULL_SCALE;
		}
		if (offset.unit == SCPI_UNIT_NONE) {
			offset.unit = SCPI_UNIT_VOLT;
		}
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
	funcgen_sin(output, freq.value, ampl.value, offset.value);
	return SCPI_RES_OK;
}

scpi_result_t dscpi_apply_sin1(scpi_t *context)
{
	return dscpi_apply_sin_inner(context, 0);
}

scpi_result_t dscpi_apply_sin2(scpi_t *context)
{
	return dscpi_apply_sin_inner(context, 1);
}

static const struct _output_mode output_modes[] = {
	{.name = "SIN", .mode = OUTPUT_MODE_SINE,},
	{.name = "TRI", .mode = OUTPUT_MODE_TRIANGLE,},
	{.name = "SQU", .mode = OUTPUT_MODE_SQUARE,},
	{.name = "USER", .mode = OUTPUT_MODE_USER,},
	{.name = NULL, .mode = OUTPUT_MODE_NULL,},
};

static const char* get_output_mode_name(enum _funcgen_output_mode output_mode)
{
	for (int i = 0; output_modes[i].name != NULL; i++) {
		if (output_modes[i].mode == output_mode) {
			return output_modes[i].name;
		}
	}
	return NULL;
}

static int dscpi_applyQ_inner(scpi_t *context, int output)
{
	printf("applyQ[%d]", output + 1);
	struct funcgen_state_t *fs;
	fs = funcgen_getstate();
	struct funcgen_output_t *outp = fs->outputs[output];
	SCPI_ResultString(context, get_output_mode_name(outp->mode));

	scpi_number_t nn;
	nn.type = SCPI_NUM_NUMBER;

	nn.unit = SCPI_UNIT_HERTZ;
	nn.value = outp->freq;
	SCPI_NumberToStr(context, &nn, numbuf, sizeof (numbuf));
	SCPI_ResultText(context, numbuf);

	nn.unit = SCPI_UNIT_VOLT;
	nn.value = outp->ampl;
	SCPI_NumberToStr(context, &nn, numbuf, sizeof (numbuf));
	SCPI_ResultText(context, numbuf);

	nn.unit = SCPI_UNIT_VOLT;
	nn.value = outp->offset;
	SCPI_NumberToStr(context, &nn, numbuf, sizeof (numbuf));
	SCPI_ResultText(context, numbuf);

	return SCPI_RES_OK;
}

scpi_result_t dscpi_apply1Q(scpi_t *context)
{
	return dscpi_applyQ_inner(context, 0);
}

scpi_result_t dscpi_apply2Q(scpi_t *context)
{
	return dscpi_applyQ_inner(context, 1);
}

/* DATA:DAC */
static scpi_result_t dscpi_data_dac_inner(scpi_t *context, int channel)
{
	// TODO
	(void) context;
	struct funcgen_output_t *fo = funcgen_getstate()->outputs[channel];
	(void) fo;
	return SCPI_RES_ERR;
}

scpi_result_t dscpi_data_dac1(scpi_t *context)
{
	return dscpi_data_dac_inner(context, 0);
}

scpi_result_t dscpi_data_dac2(scpi_t *context)
{
	return dscpi_data_dac_inner(context, 1);
}

/* SOURx:OUTP:ARB x,y (DATA:DAC?) */
static scpi_result_t dscpi_data_dacQ_inner(scpi_t *context, int channel)
{
	// TODO
	(void) context;
	struct funcgen_output_t *fo = funcgen_getstate()->outputs[channel];
	(void) fo;
	return SCPI_RES_ERR;
}

scpi_result_t dscpi_data_dac1Q(scpi_t *context)
{
	return dscpi_data_dacQ_inner(context, 0);
}

scpi_result_t dscpi_data_dac2Q(scpi_t *context)
{
	return dscpi_data_dacQ_inner(context, 1);
}


/* end scpi handlers ---------------------*/

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

//static scpi_reg_val_t scpi_regs[SCPI_REG_COUNT];

static scpi_t scpi_context = {
	.cmdlist = scpi_commands,
	.buffer =
	{
		.length = SCPI_INPUT_BUFFER_LENGTH,
		.data = scpi_input_buffer,
	},
	.interface = NULL, /* Provided by platforms */
	//.registers = scpi_regs,
	.units = scpi_units_def,
	.special_numbers = scpi_special_numbers_def,
	.idn =
	{"Ekta labs", "DiscoTMC-F4", NULL, "0.1"},
};

void dscpi_init(scpi_interface_t *intf, char *serial_number)
{
	scpi_context.interface = intf;
	scpi_context.idn[2] = serial_number;
	SCPI_Init(&scpi_context);
}

void hexdump(char* prefix, uint8_t *buf, uint16_t len)
{
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
	SCPI_Input(&scpi_context, (char *) buf, len);
	if (final) {
		SCPI_Input(&scpi_context, NULL, 0);
	}
}

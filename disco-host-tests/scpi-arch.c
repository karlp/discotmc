#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

char *secret_output;

size_t myWrite(scpi_t * context, const char * data, size_t len);

/* From Bjarni 
 
 * This is basically what I am sending

outp2 off				// make sure that output 2 is of (just because it should not be, I am only using output 1)
outp1:load inf				//set the output resistanse to none (not 50ohm)
sour1:appl:sin 50Hz,0.465Vrms,0	//set ouput 1 to appropriate signal for mid range value to be used for calibration

 * 
 */

const char *output_channels[] = {"AA", "BB", NULL};
const char *functions_gwinstek_afg2225[] = {
	"SINusoid",  /* freq, amp, offset */
	"SQUare", /* freq, amp, offset */
	"RAMP", /* freq, amp, offset.  To use non 50%, need to set explict */
	"PULSe", /* freq, amp, offset, also need other settings */
	"NOISe", /* DEF, amp, offset */
	"USER", 
	NULL
};

struct _funcgen_state {
	bool outputs[2];
	//char *load[2]; // Can be thigns like DEF, MAX; MIN, etc...
	int load[2]; // Can be thigns like DEF, MAX; MIN, etc...
	int functions[2];
	/* for freq, I really want a "SCPI Number" not an int or anything... */
	char *freq[2];
};

static struct _funcgen_state fg_state;

int dscpi_output(scpi_t *context) {
	int output;
	printf("attempting to parse an OUTP[1|2] command\n");
	bool all_outputs = false;
	// TODO use a CHOICE
	if (!SCPI_ParamInt(context, &output, false)) {
		printf("acting on both outputs\n");
		all_outputs = true;
	} else {
		printf("acting on output %d\n", output);
	}
	bool action;
	if (!SCPI_ParamBool(context, &action, true)) {
		return SCPI_RES_ERR;
	}
	printf("turning output %s", action ? "on" : "off");
	
}

int dscpi_outputQ(scpi_t *context) {
	int output;
	printf("parsing outputQ\n");
	if (!SCPI_ParamChoice(context, output_channels, &output, false)) {
		output = 0;
	}
	printf("acting on output %d (%s)\n", output, output_channels[output]);
	SCPI_ResultBool(context, fg_state.outputs[output]);
	return SCPI_RES_OK;
}

int dscpi_output_loadQ(scpi_t *context) {
	/* TODO Sort out the channel selction again! */
	int output = 0;
	printf("load parameter given, presenting saved load\n");
	//SCPI_ResultString(context, SCPI_NumberToStr()
	SCPI_ResultInt(context, fg_state.load[output]);
	return SCPI_RES_OK;
}

int dscpi_apply(scpi_t *context) {
	
}

int dscpi_applyQ(scpi_t *context) {
	
}

int dscpi_error(scpi_t * context, int_fast16_t err) {
    (void) context;

    fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int32_t) err, SCPI_ErrorTranslate(err));
    return 0;
}



scpi_command_t scpi_commands[] = {
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    { .pattern = "*RST", .callback = SCPI_CoreRst,},
//    { .pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
    //{ .pattern = "OUTPut[1|2]", .callback= dscpi_output,},
    //{ .pattern = "OUTPut [1|2]", .callback= dscpi_output,},
    //{ .pattern = "OUTPut[1|2]?", .callback= dscpi_outputQ,},
    //{ .pattern = "OUTPut1?", .callback= dscpi_outputQ,},
    //{ .pattern = "OUTPut2?", .callback= dscpi_outputQ,},
    { .pattern = "OUTPut?", .callback= dscpi_outputQ,},
    { .pattern = "OUTPut:LOAD?", .callback= dscpi_output_loadQ,},
    //{ .pattern = "OUTPut[<n>]:load?", .callback= dscpi_outputQ,},
    //{ .pattern = "OUTPut[2]", .callback= dscpi_output,},
//    {.pattern = "[SOURce]:APPLy", .callback= dscpi_apply,},
//    {.pattern = "[SOURce]:APPLy?", .callback= dscpi_applyQ,},
    {.pattern = ":APPLy", .callback= dscpi_apply,},
    {.pattern = ":APPLy?", .callback= dscpi_applyQ,},
    SCPI_CMD_LIST_END
};


scpi_interface_t scpi_interface = {
    .write = myWrite,
    .error = dscpi_error,
    .reset = NULL,
    .test = NULL,
    .control = NULL,
};

scpi_t scpi_context = {
    .cmdlist = scpi_commands,
    .buffer = {
        .length = SCPI_INPUT_BUFFER_LENGTH,
        .data = scpi_input_buffer,
    },
    .interface = &scpi_interface,
//    .registers = scpi_regs,
    .units = scpi_units_def,
    .special_numbers = scpi_special_numbers_def,
    .idn = {"hohoMANU", "kproduct", NULL, "kserial"}
};

// TODO Should be ok to be static?
size_t myWrite(scpi_t * context, const char * data, size_t len) {
    (void) context;
    printf("<scpi_write: %s>\n", data);
    memcpy(secret_output, data, len);
    secret_output[len] = '\0';
    return len;
//    return fwrite(data, 1, len, stdout);
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
	printf(">\n");
}


void scpi_init(void) {

	SCPI_Init(&scpi_context);
	fg_state.outputs[0] = true;
	fg_state.outputs[1] = false;
	fg_state.load[0] = 25;
	fg_state.load[1] = 50;
}

void scpi_glue_input(uint8_t *buf, uint16_t len, bool final, char *magic)
{
	printf("printf feeding scpi: <%s>\n", buf);
	hexdump("feeding to scpi", buf, len);
	secret_output = magic;
	SCPI_Input(&scpi_context, (char *)buf, len);
	if (final) {
		SCPI_Input(&scpi_context, NULL, 0);
	}
}

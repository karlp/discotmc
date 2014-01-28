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

struct _funcgen_state {
	bool outputs[2];
	int load[2];
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
	bool all_outputs = false;
	const char *options[] = {"11", "22", NULL};
	printf("parsing outputQ\n");
	if (!SCPI_ParamChoice(context, options, &output, false)) {
		printf("selected both outputs");
		all_outputs = true;
	} else {
		printf("acting on output %d (%s)\n", output, options[output]);
	}
	char loadstr[] = "load";
	size_t loadstr_len = sizeof(loadstr);
	if (!SCPI_ParamString(context, (const char**)&loadstr, &loadstr_len, false)) {
		printf("No load parameter passed\n");
		if (all_outputs) {
			SCPI_ResultBool(context, fg_state.outputs[0]);
			SCPI_ResultBool(context, fg_state.outputs[1]);
		} else {
			SCPI_ResultBool(context, fg_state.outputs[output]);
		}
	} else {
		printf("load parameter given, making up a load\n");
		if (all_outputs) {
			SCPI_ResultInt(context, fg_state.load[0]);
			SCPI_ResultInt(context, fg_state.load[1]);
		} else {
			SCPI_ResultInt(context, fg_state.load[output]);
		}		
	}
	return SCPI_RES_OK;
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
    //{ .pattern = "OUTPut[2]", .callback= dscpi_output,},
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
	fg_state.load[2] = 50;
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

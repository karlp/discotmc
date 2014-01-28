#include <stdio.h>
#include <string.h>
#include "scpi/scpi.h"

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];


size_t myWrite(scpi_t * context, const char * data, size_t len);


scpi_command_t scpi_commands[] = {
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    { .pattern = "*RST", .callback = SCPI_CoreRst,},
//    { .pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
    SCPI_CMD_LIST_END
};


scpi_interface_t scpi_interface = {
    .write = myWrite,
    .error = NULL,
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


size_t myWrite(scpi_t * context, const char * data, size_t len) {
    (void) context;
    return fwrite(data, 1, len, stdout);
}

int main(int argc, char **argv) {

	SCPI_Init(&scpi_context);

	//char ibuf[] = {0x2a, 0x49, 0x44, 0x4e, 0x3f, '\r', '\n'};
	char ibuf[] = {0x2a, 0x49, 0x44, 0x4e, 0x3f};
	SCPI_Input(&scpi_context, ibuf, sizeof(ibuf));
	SCPI_Input(&scpi_context, NULL, 0);

}
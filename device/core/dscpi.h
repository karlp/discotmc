/* 
 * File:   scpi.h
 * Author: karlp
 *
 * Created on January 26, 2014, 10:08 PM
 */

#ifndef KSCPI_H
#define	KSCPI_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <scpi/scpi.h>

	// Call this to feed data into the scpi blobs
	void scpi_glue_input(uint8_t *buf, uint16_t len, bool final);
	void dscpi_init(scpi_interface_t *intf, char *serial_number);
	scpi_t * dscpi_get_context(void);

	/* Platform code needs to provide these */
	void scpi_init_platform(char *serial_number);
	size_t scpi_impl_write(scpi_t * context, const char * data, size_t len);
	int scpi_impl_error(scpi_t * context, int_fast16_t err);
	/* these are optional */
	scpi_result_t scpi_impl_reset(scpi_t *context);
	scpi_result_t scpi_impl_flush(scpi_t *context);
	scpi_result_t scpi_impl_control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
	scpi_result_t scpi_impl_test(scpi_t * context);

	/* temp shit*/
	void hexdump(char* prefix, uint8_t *buf, uint16_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* KSCPI_H */


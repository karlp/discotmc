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
	void dscpi_init(scpi_interface_t *intf);

	/* Platform code needs to provide these */
	void scpi_init_platform(void);
	size_t scpi_impl_write(scpi_t * context, const char * data, size_t len);
	int scpi_impl_error(scpi_t * context, int_fast16_t err);
	scpi_result_t scpi_impl_reset(scpi_t *context);


	/* temp shit*/
	void hexdump(char* prefix, uint8_t *buf, uint16_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* KSCPI_H */


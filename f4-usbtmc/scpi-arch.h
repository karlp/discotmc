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


	// Call this to feed data into the scpi blobs
	void scpi_glue_input(uint8_t *buf, uint16_t len, bool final);
	void scpi_init(void);



#ifdef	__cplusplus
}
#endif

#endif	/* KSCPI_H */


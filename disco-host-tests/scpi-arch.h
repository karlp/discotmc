/* 
 * File:   scpi-arch.h
 * Author: karlp
 *
 * Created on January 28, 2014, 9:11 PM
 */

#ifndef SCPI_ARCH_H
#define	SCPI_ARCH_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdbool.h>
	
void scpi_init(void);
void scpi_glue_input(uint8_t *buf, uint16_t len, bool final, char *magic);



#ifdef	__cplusplus
}
#endif

#endif	/* SCPI_ARCH_H */


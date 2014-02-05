/* 
 * File:   funcgen.h
 * Author: karlp
 *
 * Created on February 5, 2014, 8:09 PM
 */

#ifndef FUNCGEN_H
#define	FUNCGEN_H

#ifdef	__cplusplus
extern "C" {
#endif
	/* TODO - put this somewhere more useful */
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))


	struct funcgen_state_t {
		bool outputs[2];
		int freq[2];
	};

	void funcgen_init_arch(void);
	/* gross api! */
	void funcgen_go(int channel, int frequency);
	void funcgen_stop(int channel);
	struct funcgen_state_t * funcgen_getstate(void);


#ifdef	__cplusplus
}
#endif

#endif	/* FUNCGEN_H */


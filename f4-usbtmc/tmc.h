/* 
 * File:   tmc.h
 * Author: karlp
 *
 * USB TMC definitions
 */

#ifndef TMC_H
#define	TMC_H

#ifdef	__cplusplus
extern "C" {
#endif

	/* Definitions of Test & Measurement Class from
	 * "Universal Serial Bus Test and Measurement Class
	 * Revision 1.0"
	 */

	/* Table 43: TMC Class Code */
	/*
	 * Application-Class” class code, assigned by USB-IF. The Host must 
	 * not load a USBTMC driver based on just the bInterfaceClass field.
	 */
#define USB_CLASS_APPLICATION		0xfe
#define USB_APPLICATION_SUBCLASS_TMC	0x03

	/* Table 44 */
#define USB_TMC_PROTOCOL_NONE		0
#define USB_TMC_PROTOCOL_USB488		1

	/* USB TMC Class Specific Requests, Table 15 sec 4.2.1 */
	/* These are all required */
#define USB_TMC_REQ_INITIATE_ABORT_BULK_OUT		1
#define USB_TMC_REQ_CHECK_ABORT_BULK_OUT_STATUS		2
#define USB_TMC_REQ_INITIATE_ABORT_BULK_IN		3
#define USB_TMC_REQ_CHECK_ABORT_BULK_IN_STATUS		4
#define USB_TMC_REQ_INITIATE_CLEAR			5
#define USB_TMC_REQ_CHECK_CLEAR_STATUS			6
#define USB_TMC_REQ_GET_CAPABILITIES			7
#define USB_TMC_REQ_INDICATOR_PULSE			64 /* optional */
	 

	/* USB TMC status values Table 16 */
#define USB_TMC_STATUS_SUCCESS				1
#define USB_TMC_STATUS_PENDING				2
#define USB_TMC_STATUS_FAILED				0x80
#define USB_TMC_STATUS_TRANSFER_NOT_IN_PROGRESS		0x81
#define USB_TMC_STATUS_SPLIT_NOT_IN_PROGRESS		0x82
#define USB_TMC_STATUS_SPLIT_IN_PROGRESS		0x83

#define USB_TMC_INTERFACE_CAPABILITY_INDICATOR_PULSE	(1<<2)
#define USB_TMC_INTERFACE_CAPABILITY_TALK_ONLY		(1<<1)
#define USB_TMC_INTERFACE_CAPABILITY_LISTEN_ONLY	(1<<0)
#define USB_TMC_DEVICE_CAPABILITY_TERMCHAR		(1<<0)

#ifdef	__cplusplus
}
#endif

#endif	/* TMC_H */


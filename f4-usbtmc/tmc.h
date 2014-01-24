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

	struct usb_tmc_bulk_header {
		uint8_t MsgID;
		uint8_t bTag;
		uint8_t bTagInverse;
		uint8_t reserved;

		union {

			struct _dev_dep_msg_out {
				uint32_t transferSize;
				uint8_t bmTransferAttributes;
				uint8_t reserved[3];
			} dev_dep_msg_out;

			struct _req_dev_dep_msg_in {
				uint32_t transferSize;
				uint8_t bmTransferAttributes;
				uint8_t TermChar;
				uint8_t reserved[2];
			} req_dev_dep_msg_in;

			struct _dev_dep_msg_in {
				uint32_t transferSize;
				uint8_t bmTransferAttributes;
				uint8_t reserved[3];
			} dev_dep_msg_in;

			struct _vendor_specific_out {
				uint32_t transferSize;
				uint8_t reserved[4];
			} vendor_specific_out;

			struct _req_vendor_specific_in {
				uint32_t transferSize;
				uint8_t reserved[4];
			} req_vendor_specific_in;

			struct _vendor_specific_in {
				uint32_t transferSize;
				uint8_t reserved[4];
			} vendor_specific_in;
			uint8_t raw[8];
		} command_specific;

	} __attribute__((packed));

	/* Table 2, MsgId values */
#define USB_TMC_MSGID_OUT_DEV_DEP_MSG_OUT		1
#define USB_TMC_MSGID_OUT_REQUEST_DEV_DEP_MSG_IN	2
#define USB_TMC_MSGID_IN_DEV_DEP_MSG_IN			2
	/* Reserved for USBTMC */
#define USB_TMC_MSGID_OUT_VENDOR_SPECIFIC_OUT		126
#define USB_TMC_MSGID_OUT_REQUEST_VENDOR_SPECIFIC_IN	127
#define USB_TMC_MSGID_IN_VENDOR_SPECIFIC_IN		127
	/* Reserved for USBTMC subclass and VISA */


#ifdef	__cplusplus
}
#endif

#endif	/* TMC_H */


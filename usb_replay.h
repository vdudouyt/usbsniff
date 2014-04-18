#ifndef __USB_REPLAY_H
#define __USB_REPLAY_H

#include <stdint.h>

typedef struct urb_s {
	unsigned int type;
	unsigned int direction;
	unsigned int device_address;
	unsigned int endpoint;
	unsigned char data[2056];
	int data_size;
	long double timing; // secs since the previous URB

	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} urb_t;

#endif

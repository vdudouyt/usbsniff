/*  (c) Valentin Dudouyt, 2013 - 2014
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

void usb_init(unsigned int vid, unsigned int pid);
void perform_transfer(urb_t *urb);
void print_help_and_exit(char **argv);

#endif

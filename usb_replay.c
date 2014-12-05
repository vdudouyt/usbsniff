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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <libusb.h>
#include "common.h"
#include "usb_replay.h"
#include "error.h"
#include "y.tab.h"

libusb_device_handle *dev_handle;
libusb_context *ctx;

void usb_init(unsigned int vid, unsigned int pid) {
	libusb_device **devs;

	int r;
	ssize_t cnt;
	r = libusb_init(&ctx);
	if(r < 0) return;

	libusb_set_debug(ctx, 3);

	dev_handle = libusb_open_device_with_vid_pid(ctx, vid, pid);
	assert(dev_handle);

	if(libusb_kernel_driver_active(dev_handle, 0) == 1) {
		printf("Warning: the device is going to be detached from kernel driver\n");
		int r = libusb_detach_kernel_driver(dev_handle, 0);
		assert(r == 0);
	}
}

void print_help_and_exit(char **argv) {
	fprintf(stderr, "Usage: %s <vid>:<pid> <filename>\n", argv[0]);
	exit(-1);
}

void continue_transfer_cb(struct libusb_transfer *ptTransfer) {
}

void perform_transfer(urb_t *urb) {
	int direction = urb->direction == IN ? LIBUSB_ENDPOINT_IN : LIBUSB_ENDPOINT_OUT;

	/* Take a care about timings */
	if(urb->timing) {
		int time = urb->timing*pow(10,6);
		usleep(time);
	}

	/* Trigger libusb to perform the transfer */
	int r, bytes_transferred = 0;
	struct libusb_transfer transfer;
	switch(urb->type) {
		case CTRL:
			r = libusb_control_transfer(dev_handle, 
					urb->bmRequestType | direction,
					urb->bRequest,
					urb->wValue,
					urb->wIndex,
					urb->data,
					urb->data_size,
					0);
			ASSERT2(r >= 0, TRANSFER_FAILED_MESSAGE, libusb_error_name(r));
			break;
		case BULK:
			r = libusb_bulk_transfer(dev_handle,
					urb->endpoint | direction,
					urb->data,
					urb->data_size,
					&bytes_transferred,
					0);
			ASSERT2(r == 0, TRANSFER_FAILED_MESSAGE, libusb_error_name(r));
			break;
		case INTR:
			r = libusb_interrupt_transfer(dev_handle,
					urb->endpoint | direction,
					urb->data,
					urb->data_size,
					&bytes_transferred,
					0);
			ASSERT2(r == 0, TRANSFER_FAILED_MESSAGE, libusb_error_name(r));
			break;
		case ISOC:
			libusb_fill_iso_transfer(&transfer,
					dev_handle,
					urb->endpoint | direction,
					urb->data,
					urb->data_size,
					1,
					continue_transfer_cb,
					NULL,
					0);
			libusb_submit_transfer(&transfer);
			ASSERT2(r == 0, TRANSFER_FAILED_MESSAGE, libusb_error_name(r));
			break;
	}
}

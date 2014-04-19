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

	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
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
					urb->endpoint,
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

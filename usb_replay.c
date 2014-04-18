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
	cnt = libusb_get_device_list(ctx, &devs);
	if(cnt < 0) return;

	dev_handle = libusb_open_device_with_vid_pid(ctx, vid, pid);
	assert(dev_handle);

	libusb_free_device_list(devs, 1); //free the list, unref the devices in it

	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
		int r = libusb_detach_kernel_driver(dev_handle, 0);
		assert(r == 0);
	}
}

void print_help_and_exit(char **argv) {
	fprintf(stderr, "Usage: %s <vid>:<pid> <filename>\n", argv[0]);
	exit(-1);
}

void perform_transfer(urb_t *urb) {
	int endpoint = urb->endpoint | (urb->direction == IN ? LIBUSB_ENDPOINT_IN : LIBUSB_ENDPOINT_OUT);

	/* Take a care about timings */
	if(urb->timing) {
		int time = urb->timing*pow(10,6);
		usleep(time);
	}
	return;

	/* Trigger libusb to perform the transfer */
	int r, bytes_transferred = 0;
	if(urb->type == CTRL && urb->direction == OUT) {
		r = libusb_control_transfer(dev_handle, 
				urb->bmRequestType,
				urb->bRequest,
				urb->wValue,
				urb->wIndex,
				urb->data,
				urb->wLength,
				0);
	} else if(urb->type == CTRL && urb->direction == IN) {
		r = libusb_control_transfer(dev_handle, 
				0,
				0,
				0,
				0,
				urb->data,
				urb->data_size,
				0);
	} else if(urb->type == BULK) {
		r = libusb_bulk_transfer(dev_handle,
				endpoint,
				urb->data,
				urb->data_size,
				&bytes_transferred,
				0);
	} else if(urb->type == INTR) {
		r = libusb_intr_transfer(dev_handle,
				endpoint,
				urb->data,
				urb->data_size,
				&bytes_transferred,
				0);
	}

	/* Exception handling */
	if(r != 0) {
		ERROR2("Transfer failed: %s\n", libusb_error_name(r));
	}
}

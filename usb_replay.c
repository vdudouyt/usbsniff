#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <libusb.h>
#include "common.h"

libusb_device_handle *dev_handle;
libusb_context *ctx;

typedef struct urb_s {
	int initialized : 1; // set by read_urb on successful read
	enum {
		CONTROL,
		INTERRUPT,
		ISOCHRONOUS,
		BULK,
	} type;
	enum {
		OUT,
		IN,
	} direction;
	unsigned int device_address;
	unsigned int endpoint;
	unsigned char data[2056];
	int data_size;
	long double timing; // secs since the previous URB
} urb_t;

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

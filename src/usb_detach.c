#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libusb.h>
#include <assert.h>

// Building: gcc usb_detach.c  -o usb_detach `pkg-config --cflags --libs libusb-1.0`

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

void print_help_and_exit() {
	exit(-1);
}

int main(int argc, char **argv) {
	if(argc != 3)
		print_help_and_exit();
	int success, vid, pid;
	success = sscanf(argv[1], "%x", &vid);
	assert(success);
	success = sscanf(argv[2], "%x", &pid);
	assert(success);
	usb_init(vid, pid);
	libusb_close(dev_handle);
}

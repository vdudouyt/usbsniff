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

	dev_handle = libusb_open_device_with_vid_pid(ctx, vid, pid);
	assert(dev_handle);

	libusb_detach_kernel_driver(dev_handle, 0);
	assert(r == 0);
}

void print_help_and_exit(char **argv) {
	fprintf(stderr, "Usage: %s <vid>:<pid>\n", argv[0]);
	exit(-1);
}

int main(int argc, char **argv) {
	if(argc != 2)
		print_help_and_exit(argv);
	int result, vid, pid;
	result = sscanf(argv[1], "%x:%x", &vid, &pid);
	assert(result);
	usb_init(vid, pid);
	libusb_close(dev_handle);
}

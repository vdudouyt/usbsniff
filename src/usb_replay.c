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
}

void print_help_and_exit(char **argv) {
	fprintf(stderr, "Usage: %s <vid>:<pid> <filename>\n", argv[0]);
	exit(-1);
}

int trim_whitespace(char *str) {
	int i;
	for(i = strlen(str); i > 0; i--) {
		if(isspace(str[i]))
			str[i] = '\0';
	}
}

int read_urb(char *line, urb_t *out) {
	int i;
	memset(out, 0, sizeof(urb_t));
	while(isspace(line[0]))	line++; // Strip the leading spaces
	// Ignore if it starts with comment sign
	if(line[0] == '#')
		return(1);
	// Ensure that it's enough long for safe operation
	if(strlen(line) < 14)
		return(0);

	// Reading the URB transfer type
	char transfer_type[5];
	strncpy(transfer_type, line, 4);
	transfer_type[4] = '\0';
	if(!strcmp(transfer_type, "ISOC")) {
		out->type = ISOCHRONOUS;
	} else if(!strcmp(transfer_type, "INTR")) {
		out->type = INTERRUPT;
	} else if(!strcmp(transfer_type, "CTRL")) {
		out->type = CONTROL;
	} else if(!strcmp(transfer_type, "BULK")) {
		out->type = BULK;
	} else {
		return(0);
	}
	line += strlen(transfer_type); // Mark as read
	line++; // Trailing underscore

	// Reading the direction
	char str_out[] = "OUT";
	char str_in[] = "IN";
	if(!memcmp(line, str_out, strlen(str_out))) {
		out->direction = OUT;
		line += strlen(str_out);
	} else if(!memcmp(line, str_in, strlen(str_in))) {
		out->direction = IN;
		line += strlen(str_in);
	} else {
		return(0); // Fail
	}

	// Reading the endpoint
	int device_address, endpoint_number;
	if(!sscanf(line, "(%d.%d):", &device_address, &endpoint_number))
		return 0;
	if (!(line = strchr(line, ':')))
		return 0;
	line++; // Trailing colon
	while(isspace(line[0]))	line++; // Strip the leading spaces

	// Read the timing (secs since previous URB) if specified
	char *comment = strchr(line, '#'); // Does line contains the sharp sign?
	if(comment) {
		comment[0] = '\0'; // Split from line
		comment++;
		while(isspace(comment[0])) comment++; // Strip the leading spaces
			if(!sscanf(comment, "%Lf", &(out->timing)))
				return 0;
	}
	trim_whitespace(line);
	assert(strlen(line) % 2 == 0); // Total nibbles count should be even
	out->data_size = hex_to_buf(line, out->data);
	out->initialized = 1;
	return(1);
}

int main(int argc, char **argv) {
	if(argc != 3)
		print_help_and_exit(argv);
	int result, vid, pid;
	result = sscanf(argv[1], "%x:%x", &vid, &pid);
	usb_init(vid, pid);

	char line[4115];
	int r, actual;
	FILE *file = fopen ( argv[2], "r" );
	assert(file);
	urb_t previous, current;
	memset(&previous, 0, sizeof(urb_t));
	char *read_success = NULL;
	do {
		read_success = fgets ( line, sizeof line, file ); // Reading line-by-line
		memset(&current, 0, sizeof(urb_t));
		int result = read_urb(line, &current);
		if(!result) {
			fprintf(stderr, "Failed while parsing line: %s\n", line);
			continue;
		}
		if(previous.initialized) {
			assert(previous.type == BULK); // The only one that's currently supported
			if(current.timing) {
				int time = current.timing*pow(10,6);
				usleep(time);
			}
			switch(previous.direction) {
				case OUT:
					printf("out length: %d\n", previous.data_size);
					r = libusb_bulk_transfer(dev_handle,
						(2 | LIBUSB_ENDPOINT_OUT),
						previous.data,
						previous.data_size,
						&actual,
						0);
					assert(r == 0);
					break;
				case IN:
					r = libusb_bulk_transfer(dev_handle,
						(1 | LIBUSB_ENDPOINT_IN),
						previous.data,
						previous.data_size,
						&actual,
						0);
					assert(r == 0);
					break;
			}
		}
		// Send on the next iteration
		memcpy(&previous, &current, sizeof(urb_t));
	} while ( read_success );

	libusb_close(dev_handle);
}

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <pcap/usb.h>
#include <libusb.h>
#include <assert.h>
#include "common.h"

void print_help_and_exit(char **argv) {
	fprintf(stderr, "Usage: %s <vid>:<pid> [<filename>]\n", argv[0]);
	exit(-1);
}

libusb_device *find_usb_device(libusb_context *ctx, unsigned int vid, unsigned int pid) {
	libusb_device **devs;
	int devices_count = libusb_get_device_list(ctx, &devs); // All devices in system
	// Looking for device with desired vid / pid
	struct libusb_device_descriptor desc;
	int i;
	for(i = 0; i < devices_count; i++) {
		int result = libusb_get_device_descriptor(devs[i], &desc);
		assert(result >= 0);
		if(desc.idVendor == vid && desc.idProduct == pid)
			return(devs[i]);
	}
	return(NULL); // default
}

unsigned char bus_number, device_number;
char errbuf[PCAP_ERRBUF_SIZE];
void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
	char hex[8192];
//	buf_to_hex(packet, header->len, &hex);
//	printf("Packet data: %s\n", hex);
	#ifdef LINUX
	pcap_usb_header *usb_header = packet;
	if(usb_header->device_address != device_number)
		return;
	printf("device_address=%d endpoint_number=%x, len=%d urb_len=%d data_len=%d bmRequestType=%d\n",
			usb_header->device_address,
			usb_header->endpoint_number,
			header->len,
			usb_header->urb_len,
			usb_header->data_len,
			usb_header->setup.bmRequestType);
	if(usb_header->data_len) {
		unsigned char *raw_data = packet + header->len - usb_header->data_len;
		buf_to_hex(raw_data, usb_header->data_len, hex);
		printf("hex: %s\n", hex);
	}
	#endif
}

void pcap_list_devices() {
	pcap_if_t *alldevs, *d;
	int result = pcap_findalldevs(&alldevs, errbuf);
	assert(result != -1);
	// Print the list
	for(d= alldevs; d != NULL; d= d->next)
	{
		printf("%s\n", d->name);
	}
	pcap_freealldevs(alldevs);
}

int main(int argc, char *argv[])
{
	// Parsing args
	if(argc < 2)
		print_help_and_exit(argv);
	int result, vid, pid;
	result = sscanf(argv[1], "%x:%x", &vid, &pid);
	assert(result);

	// Dealing with libusb
	libusb_device_handle *dev_handle;
	libusb_context *ctx;
	result = libusb_init(&ctx); // Obtain libusb_context
	assert(result >= 0);
	libusb_set_debug(ctx, 3);
	libusb_device *usbdev = find_usb_device(ctx, vid, pid);
	assert(usbdev);
	bus_number = libusb_get_bus_number(usbdev); // Which bus to sniff
	device_number = libusb_get_device_address(usbdev); // Filter
	fprintf(stderr, "bus_number=%d, device_number=%d\n", bus_number, device_number);

	char pcap_if_name[10];
	#ifdef LINUX
		sprintf(pcap_if_name, "usbmon%d", bus_number);
	#else 
		#error "Unsupported platform"
	#endif

	printf("pcap device: %s\n", pcap_if_name);
	pcap_t *handle = pcap_open_live(pcap_if_name, BUFSIZ, 1, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", pcap_if_name, errbuf);
		#ifdef LINUX
		fprintf(stderr, "Please check for modules list.\n");
		#endif
		return(2);
	}
	pcap_loop(handle, -1, process_packet, NULL);
	printf("exit\n");
	pcap_close(handle);
}

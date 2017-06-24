/*  (c) Valentin Dudouyt, 2013 - 2017
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

/* usbreset -- send a USB port reset to a USB device 
 * based on code by Alan Stern (http://marc.info/?l=linux-usb&m=121459435621262&w=2) */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <libusb.h>
#include <stdlib.h>

void print_help_and_exit(char **argv) {
   fprintf(stderr, "Usage: %s <vid>:<pid> [<filename>]\n", argv[0]);
   exit(-1);
}

int main(int argc, char **argv)
{
    const char *filename;
    int fd;
    int rc;

    if (argc != 2)
      print_help_and_exit(argv);

    int result, vid, pid;
    result = sscanf(argv[1], "%x:%x", &vid, &pid);

    if(!result)
       print_help_and_exit(argv);

    libusb_init(NULL);
    libusb_device_handle *handle = libusb_open_device_with_vid_pid(NULL, vid, pid);

    if(!handle) {
        fprintf(stderr, "Couldn't open device\n");
        return 1;
    }

    result = libusb_reset_device(handle);

    if(!handle) {
        fprintf(stderr, "Couldn't reset device\n");
        return 1;
    }

    return 0;
}

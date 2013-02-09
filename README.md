usbsniff
=========

USB reverse engineering utils (capture / replay / e.t.c.)

Synopsis:

1) Dump the data in Human-Readable Format:

usb_capture 1234:4567 data.cap

2) Send the data back to device (with taking care about timings):

usb_replay 1234:4567 data.cap

3) Detach the device from kernel module:

usb_detach 1234:4567

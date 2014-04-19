usbsniff
=========

Taking the reverse engineering seriously

### Synopsis

```nohiglight
$ usb_capture 1234:4567 data.cap
$ usb_replay 1234:4567 data.cap
$ usb_detach 1234:4567
$ usb_reset 1234:4567
```

### Output Format

```nohiglight
# transfer_type:bmRequestType:bRequest:wValue:wIndex:wLength:data
CTRL_OUT(0.0):21:01:0200:0600:0005:80b8000000
CTRL_IN(0.0): 00:00:0000:0000:0000:0000 # 0.0030059999553487
# transfer_type:endpoint:data
BULK_OUT(0.1):80b8000000 # 0.0028590000001714
INTR_OUT(0.1):80b8000000 # 0.0028590000001714
ISOC_OUT(0.1):eeffffff00000000f40b00000000000 # 0.1025490000611171
BULK_IN(0.1): 80b8000000 # 0.0028590000001714
INTR_IN(0.1): 80b8000000 # 0.0028590000001714
ISOC_IN(0.1): eeffffff00000000f40b00000000000 # 0.1025490000611171
```

### FAQ

* How does usb_replay handles the input transfers?

  It fires an input request through libusb-1.0 which blocks until the response will be received.

* Does usb_replay respects the timings?

  Yes, it waits for a specified interval (which is specified in seconds after the sharp sign) before performing each call.

* Why do I need to replug the device after running usb_replay?
 
  It performs a detach_kernel_driver() call in the beginning, so in the case of your mouse, it may be looking like it's suddenly stopped working. This doesn't means that something is actually went wrong, and there is no need to replug until you'll need it interacting with your system again.

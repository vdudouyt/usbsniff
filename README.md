usbsniff
=========

USB reverse engineering utils (capture / replay / e.t.c.)

### Synopsis

```nohiglight
$ usb_capture 1234:4567 data.cap
$ usb_replay 1234:4567 data.cap
$ usb_detach 1234:4567
$ usb_reset 1234:4567
```

### Format

```nohiglight
# transfer_type:bmRequestType:bRequest:wValue:wIndex:wLength:data
CTRL_OUT(0.0):21:01:0200:0600:0005:80b8000000
# transfer_type:data
CTRL_IN(0.0): 0500 # 0.0030059999553487
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

  It fires an input request and blocks until the response received.

* Does usb_replay respects the timings?

  It waits for a specified interval (after sharp sign) before performing each call.

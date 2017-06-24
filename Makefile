.PHONY: all clean install
.SUFFIXES:
.SUFFIXES: .c .o .h

EXECUTABLES=usb_capture
OBJECTS=common.o
USB_REPLAY_OBJECTS=y.tab.o lex.yy.o
PROGS=usb_capture usb_detach usb_replay usb_reset
CFLAGS = `pkg-config --cflags libusb-1.0` -g -O0 -DLINUX
LIBS = `pkg-config --libs libusb-1.0` -lpcap -ll -ly

all: $(OBJECTS) $(PROGS)

clean:
	rm -f $(OBJECTS) $(PROGS) lex.yy.* y.tab.*

install:
	cp $(PROGS) $(DESTDIR)/usr/bin/

usb_capture: usb_capture.c
	$(CC) $(CFLAGS) $(OBJECTS) usb_capture.c $(LIBS) -o usb_capture

usb_detach: usb_detach.c
	$(CC) $(CFLAGS) $(OBJECTS) usb_detach.c $(LIBS) -o usb_detach

usb_reset: usb_reset.c
	$(CC) $(CFLAGS) $(OBJECTS) usb_reset.c $(LIBS) -o usb_reset

usb_replay: $(USB_REPLAY_OBJECTS) usb_replay.c
	$(CC) $(CFLAGS) $(OBJECTS) $(USB_REPLAY_OBJECTS) usb_replay.c $(LIBS) -o usb_replay

y.tab.c: usb_replay.y common.c usb_replay.c
	bison -y -d usb_replay.y

lex.yy.c: usb_replay.l
	flex usb_replay.l

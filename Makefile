.PHONY: all clean install
EXECUTABLES=usb_capture
OBJECTS=common.o
PROGS=usb_capture usb_detach usb_replay usb_reset
CFLAGS = `pkg-config --cflags libusb-1.0` -g -O0 -DLINUX
LIBS = `pkg-config --libs libusb-1.0` -lpcap

all: $(OBJECTS) $(PROGS)

clean:
	rm $(OBJECTS) $(PROGS)

install:
	cp $(PROGS) $(DESTDIR)/usr/bin/

usb_capture: usb_capture.c
	$(CC) $(CFLAGS) $(OBJECTS) usb_capture.c $(LIBS) -o usb_capture

usb_detach: usb_detach.c
	$(CC) $(CFLAGS) $(OBJECTS) usb_detach.c $(LIBS) -o usb_detach

usb_replay: usb_replay.c
	$(CC) $(CFLAGS) $(OBJECTS) usb_replay.c $(LIBS) -o usb_replay

parser: usb_replay.l usb_replay.y
	bison -y -d usb_replay.y
	flex usb_replay.l
	gcc -c y.tab.c lex.yy.c
	gcc y.tab.o lex.yy.o -o parser -ll -ly

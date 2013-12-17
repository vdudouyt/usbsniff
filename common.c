#include <stdio.h>
#include <string.h>
#include <assert.h>

void buf_to_hex(const unsigned char *buf, int len, char *str) {
	int i; 
	str[0] = '\0';
	for (i = 0; i < len; i++) {
		sprintf(&str[i*2], "%02x", (int)buf[i]);
	}
}

int hex_to_buf(const char *str, unsigned char *buf) {
	int i, d;
	assert(!(strlen(str) % 2));
	char chunk[3];
	for(i = 0; i < strlen(str); i+=2) {
		chunk[0] = str[i];
		chunk[1] = str[i+1];
		chunk[2] = '\0';
		sscanf(chunk, "%02x", &d);
		buf[i / 2] = d;
	}
	return(i / 2);
}

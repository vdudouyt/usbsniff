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

unsigned int hex_to_int(const char *str) {
	unsigned int result;
	sscanf(str, "%x", &result);
	return(result);
}

#include "include/t2fs.h"
#include "include/apidisk.h"
#include <string.h>

int identify(char *name, int size) {
	strncpy(name, "Luis Pedro Silvestrin 228528 Marcos Henrique Backes 228483", size);
	name[size - 1] = '\0';
	return 0;
}

int init() {
	char buff[SECTOR_SIZE];
	read_sector(0, buff);
	memcpy(&_sblock_, buff, 36);
}

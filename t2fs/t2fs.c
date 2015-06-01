#include "t2fs.h"
#include <string.h>

int identify(char *name, int size) {
	strncpy(name, "Luis Pedro Silvestrin xxxxxxxx Marcos Henrique Backes 228483", size);
	name[size - 1] = '\0';
	return 0;
}

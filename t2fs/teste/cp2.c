/*
 * cpyfile.c
 *
 *  Created on: Jun 25, 2015
 *      Author: mhbackes
 */

/*
 * ls2.c
 *
 *  Created on: Jun 25, 2015
 *      Author: mhbackes
 */

#include <stdio.h>
#include "../include/t2fs.h"

#define BUFFER_SIZE 1

void usage() {
	puts("Usage: cp2 srcpath dstpath");
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		usage();
		return 0;
	}

	FILE* srcf = fopen(argv[1], "rb");
	if (srcf != NULL) {
		printf("Source file %s opened.\n", argv[1]);
	} else {
		puts("Could not open source file.");
		return 0;
	}

	FILE2 dstf = create2(argv[2]);
	if (dstf >= 0) {
		printf("Destination file %s created.\n", argv[2]);
	} else {
		printf("Could not create destination file.");
		fclose(srcf);
		return 0;
	}

	char buffer[BUFFER_SIZE];

	puts("Copying file...");
	int read_status;
	while (fread(buffer, sizeof(char), BUFFER_SIZE, srcf) != 0) {
		if (write2(dstf, buffer, BUFFER_SIZE) < 0) {
			puts("Could not write bytes in file.");
		}
	}
	puts("File copied.");

	int error = fclose(srcf);
	if (error == 0) {
		printf("Source file %s closed.\n", argv[1]);
	} else {
		puts("Could not close source file.");
	}

	error = close2(dstf);
	if (error == 0) {
		printf("Destination file %s created.\n", argv[2]);
	} else {
		puts("Could not close destination file.");
	}

	return 0;
}

/*
 * rm2.c
 *
 *  Created on: Jun 25, 2015
 *      Author: mhbackes
 */

#include <stdio.h>
#include "../include/t2fs.h"

void usage(){
	puts("Usage: rm2 filename");
}

int main (int argc, char* argv[]){
	if(argc != 2){
		usage();
		return 0;
	}
	int error = delete2(argv[1]);
	if(error != 0){
		printf("File %s removed.\n", argv[1]);
	} else {
		puts("Could not remove file.");
	}
	return 0;
}

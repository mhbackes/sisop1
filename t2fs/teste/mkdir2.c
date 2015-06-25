/*
 * mkdir.c
 *
 *  Created on: Jun 25, 2015
 *      Author: mhbackes
 */

#include <stdio.h>
#include "../include/t2fs.h"

void usage(){
	puts("Usage: mkdir2 pathname");
}

int main (int argc, char* argv[]){
	if(argc != 2){
		usage();
		return 0;
	}
	int error = mkdir2(argv[1]);
	if(error != 0){
		printf("Directory %s created.\n", argv[1]);
	} else {
		puts("Could not create directory.");
	}
}

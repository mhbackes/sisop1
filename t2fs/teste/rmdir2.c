/* mkdir.c
 *
 *  Created on: Jun 25, 2015
 *      Author: mhbackes
 */

#include <stdio.h>
#include "../include/t2fs.h"

void usage(){
	puts("Usage: rmdir2 pathname");
}

int main (int argc, char* argv[]){
	if(argc != 2){
		usage();
		return 0;
	}
	int error = rmdir2(argv[1]);
	if(error != 0){
		printf("Dirrectory %s removed.\n", argv[1]);
	} else {
		puts("Could not remove directory.");
	}
}

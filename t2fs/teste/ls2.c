/*
 * ls2.c
 *
 *  Created on: Jun 25, 2015
 *      Author: mhbackes
 */

#include <stdio.h>
#include "../include/t2fs.h"

#define DIR "dir"
#define FILE "file"

void usage() {
	puts("Usage: ls2 [pathname]");
}

int main(int argc, char* argv[]) {
	char cwd[1024];
	DIR2 dir;
	DIRENT2 dir_entry;

	if(argc > 2){
		usage();
		return 0;
	}

	// troca para o diretório se especificado
	if(argc == 2) {
		chdir2(argv[1]);
	}

	int error = getcwd2(cwd, 1024);
	if (error == 0) {
		printf("Directory %s opened.\n", cwd);
	} else {
		puts("Could not get current working directory");
		return 0;
	}
	dir = opendir2(cwd);
	if (dir >= 0) {
		puts("TYPE\tSIZE\tNAME");
		while (readdir2(dir, &dir_entry) == 0) {
			printf("%s\t%ld\t%s\n", (dir_entry.fileType ? DIR : FILE), dir_entry.fileSize,
							dir_entry.name);
		}
	} else {
		puts("Could not open directory.");
	}
	error = closedir2(dir);
	if (error == 0) {
		printf("Directory %s closed.\n", cwd);
	} else {
		puts("Could not close directory.");
	}

	return 0;
}

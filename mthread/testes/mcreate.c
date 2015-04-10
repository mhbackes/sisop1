/*
 * mcreate.c
 *
 *  Created on: Apr 9, 2015
 *      Author: mhbackes
 */

#include <stdlib.h>
#include <stdio.h>
#include "../include/mthread.h"

void print_thread(){
	printf("Olá, sou uma thread!");
}

int main() {
	mcreate(0, &print_thread, NULL);

	return 0;
}

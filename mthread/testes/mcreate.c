/*
 * mcreate.c
 *
 *  Created on: Apr 9, 2015
 *      Author: mhbackes
 */

#include <stdlib.h>
#include <stdio.h>
#include "../include/mthread.h"
#include "../include/mdata.h"

void print_thread(){
	printf("Olá, sou uma thread!");
}

int main() {
	TCB_t* teste1 = thread_init(0, 1, 1, &print_thread, NULL);
	TCB_t* teste2 = thread_init(2, 1, 1, &print_thread, NULL);
	TCB_t* teste3 = thread_init(3, 1, 1, &print_thread, NULL);
	TCB_t* teste4 = thread_init(4, 1, 1, &print_thread, NULL);
	insert_blocked_thread(0, teste2);
	insert_blocked_thread(3, teste4);
	insert_blocked_thread(4, teste3);
	insert_blocked_thread(2, teste1);
	
	return 0;
}

/*
 * mcreate.c
 *
 *  Created on: Apr 9, 2015
 *      Author: mhbackes
 */

#include <stdlib.h>
#include <stdio.h>
#include "../include/mthread.h"

void print_thread_wait(int* i){
	printf("Olá, sou a thread %d! Agora vou aguardar pela thread %d.\n", i[0], i[1]);
	mwait(i[1]);
	printf("Olá, sou a thread %d! Agora que a thread %d terminou, posso terminar.\n", i[0], i[1]);
}

void print_thread_yield(int* i){
	printf("Olá, sou a thread %d! Agora vou yeldar.\n", i[0]);
	myield();
	printf("Olá, sou a thread %d! Agora que já yieldei, posso terminar.\n", i[0]);
}

int main() {
	int i[] = {1};
	int j[] = {2, 1};
	int k[] = {3};
	mcreate(0, (void *(*)(void*)) &print_thread_yield, i);
	mcreate(0, (void *(*)(void*)) &print_thread_wait, j);
	mcreate(0, (void *(*)(void*)) &print_thread_yield, k);
	printf("Olá, sou a thread main, sou mais legal! Agora vou aguardar pela thread 2.\n");
	mwait(2);
	printf("Olá, sou a thread main, sou mais legal! Agora que as outras threads plebéias já acabaram, um abraço.\n");
	return 0;
}

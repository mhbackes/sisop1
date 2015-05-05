#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"
#include "../include/mdata.h"

#define N_THREADS 10

typedef struct thread {
	int id;
	int prio;
} thread_t;

void print_thread_yield(thread_t* t){
	int i;
	printf("Thread: %d, prio: %d - executando.\n", t->id, t->prio);
	for (i = 0; i < 3; i++) {
		myield();
		printf("Thread: %d, prio: %d - yield.\n", t->id, t->prio);
	}
	printf("Thread: %d, prio: %d - terminando.\n", t->id, t->prio);
}

int main() {
	int i;
	thread_t t[N_THREADS];
	printf("Main criando 10 threads...\n");
	for (i = 0; i < N_THREADS; i++) {
		t[i].prio = i%3;
		t[i].id = mcreate((i%3), (void *(*)(void*)) &print_thread_yield, &t[i]);
	}
	printf("Main esperando as threads...\n");
	for (i = 0; i < N_THREADS; i++) {
		mwait(t[i].id);
	}
	
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

#define N_THREADS 6

typedef struct thread {
	int id;
	int prio;
} thread_t;

void print_thread(thread_t* t){
	printf("Thread: %d, prio: %d - executando.\n", t->id, t->prio);
}

int main() {
	int i;
	thread_t t[N_THREADS];
	printf("Mcreate com prioridade -1: retorno %d\n", mcreate(-1, (void *(*)(void*)) &print_thread, NULL));
	printf("Mcreate com prioridade 3: retorno %d\n", mcreate(3, (void *(*)(void*)) &print_thread, NULL));
	
	printf("Main criando 6 threads...\n");
	for (i = 0; i < N_THREADS; i++) {
		t[i].prio = i%3;
		t[i].id = mcreate((i%3), (void *(*)(void*)) &print_thread, &t[i]);
	}
	printf("Main esperando as threads...\n");
	for (i = 0; i < N_THREADS; i++) {
		mwait(t[i].id);
	}
	printf("Fim da funcao main...\n");
	
	return 0;
}

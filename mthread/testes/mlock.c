#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

#define N_THREADS 3

mmutex_t mutex;

typedef struct thread {
	int id;
	int prio;
} thread_t;

void print_thread_mutex(thread_t* t){
	printf("Thread: %d, prio: %d - iniciando.\n", t->id, t->prio);
	mlock(&mutex);
	printf("Thread: %d, prio: %d - entrou no mutex.\n", t->id, t->prio);
	myield();
	munlock(&mutex);
	printf("Thread: %d, prio: %d - saiu do mutex.\n", t->id, t->prio);		
}

int main() {
	int i;
	thread_t t[N_THREADS];
	mmutex_init(&mutex);
	
	printf("mlock executado antes de iniciar escalonador retorna %d.\n",
				mlock(&mutex));
	
	printf("Main criando %d threads...\n", N_THREADS);
	for (i = 0; i < N_THREADS; i++) {
		t[i].prio = 1;
		t[i].id = mcreate(1, (void *(*)(void*)) &print_thread_mutex, &t[i]);
	}
	printf("Main esperando as threads...\n");
	for (i = 0; i < N_THREADS; i++) {
		mwait(t[i].id);
	}
	printf("Fim da funcao main...\n");
	
	return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include "../include/mthread.h"

#define NUM_THREADS 6

typedef struct thread {
	int id;
	int prio;
} thread_t;

void waiting_thread(thread_t* t){
	printf("Thread %d de prioridade %d aguardando thread %d.\n", t->id, t->prio, t->id+1);
	mwait(t->id+1);
	printf("Thread %d terminando.\n", t->id);
}

void yield_thread(thread_t* t){
	printf("Thread %d yield\n", t->id);
	myield();
	printf("Thread %d terminando.\n", t->id);
}

int main() {
	thread_t t[NUM_THREADS];
	int i;
	int ret = mwait(10);
	printf("Esperando thread inexistente, retorno: %d\n", ret);
	
	for (i = 0; i < NUM_THREADS-1; i++) {
		t[i].prio = i%3;
		t[i].id = mcreate((i%3), (void *(*)(void*)) &waiting_thread, &t[i]);
	}
	t[i].prio = 2;
	t[i].id = mcreate(2, (void *(*)(void*)) &yield_thread, &t[i]);
	for (i = 0; i < NUM_THREADS; i++) {
		mwait(t[i].id);
	}
	ret = mwait(0);
	printf("mwait para a prórpia thread retora: %d\n", ret);
	return 0;
}

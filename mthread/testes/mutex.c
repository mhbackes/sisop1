#include <stdlib.h>
#include <stdio.h>
#include "../include/mthread.h"

#define N_THREADS 7

mmutex_t* mutex;

void print_thread_mutex(int* i) {
	printf("Olá, sou a thread %d! Agora vou entrar no mutex.\n", i[0]);
	mlock(mutex);
	printf(
			"Olá, sou a thread %d! Estou dentro do mutex. Que solidão! Vou yeldar agora pra ver se acho companhia.\n",
			i[0]);
	myield();
	printf(
			"Olá, sou a thread %d! Agora que já yieldei, posso sair desse mutex.\n",
			i[0]);
	munlock(mutex);
	printf("Olá, sou a thread %d! Agora que saí do mutex, posso terminar.\n",
			i[0]);
}

int main() {
	int tids[N_THREADS];
	int i;

	mutex = malloc(sizeof(mmutex_t));
	mmutex_init(mutex);
	munlock(mutex);

	for (i = 0; i < N_THREADS; ++i)
		tids[i] = mcreate(0, (void *(*)(void*)) &print_thread_mutex, &tids[i]);

	printf(
			"Olá, sou a thread main, sou mais legal! Agora vou aguardar por todas as outras threads.\n");

	for (i = 0; i < N_THREADS; i++) {
		printf(
				"Olá, sou a thread main, sou mais legal! Agora estou aguardando pela thread %d.\n",
				tids[i]);
		mwait(tids[i]);
	}

	printf(
			"Olá, sou a thread main, sou mais legal! Agora que as outras threads plebéias já acabaram, um abraço.\n");
	free(mutex);
	return 0;
}

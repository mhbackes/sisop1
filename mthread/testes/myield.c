
/*
 *  Teste myield
 *
 *  Nesse teste são criadas cinco threads com prioridades variadas. A
 *  prioridade de cada thread equivale ao módulo 3 de sua ordem de criação,
 *  que varia de zero a cinco. Cada thread criada executa um myield,
 *  fazendo com que as threads de prioridades iguais se alternem até acabar.
 *  Assim, as threads de prioridade maior terminam primeiro e as de menor
 *  prioridade terminam por último. Antes de criar as threads, a main executa
 *  um myield, que retorna -1, pois o escalonador ainda não foi inicializado.
 *  Depois que as threads terminaram, o programa executa  myield outra vez,
 *  e o retorno será 0, pois o escalonador já está inicializado. O programa
 *  não recebe nenhum parâmetro e imprime na tela a operação realizada por
 *  cada thread (início, \emph{myield} e término).
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

#define N_THREADS 5

typedef struct thread {
	int id;
	int prio;
} thread_t;

void print_thread_yield(thread_t* t){
	printf("Thread: %d, prio: %d - executando.\n", t->id, t->prio);
	printf("Thread: %d, prio: %d - yield.\n", t->id, t->prio);
	myield();
	printf("Thread: %d, prio: %d - terminando.\n", t->id, t->prio);
}

int main() {
	int i;
	int ret = myield();
	printf("Testando myield no main sem outras threads, retorno: %d\n", ret);	
	thread_t t[N_THREADS];
	printf("Main criando %d threads...\n", N_THREADS);
	for (i = 0; i < N_THREADS; i++) {
		t[i].prio = i%3;
		t[i].id = mcreate((i%3), (void *(*)(void*)) &print_thread_yield, &t[i]);
	}
	printf("Main esperando as threads...\n");
	for (i = 0; i < N_THREADS; i++) {
		mwait(t[i].id);
	}
	ret = myield();
	printf("myield do main após o término de todas as threads retorna %d\n", ret);
	return 0;
}


/*
 *  Teste primos
 * 
 *  Nesse teste são criadas cinco threads com prioridades variadas. A
 *  prioridade de cada thread equivale ao módulo 3 de sua ordem de criação, que
 *  varia de zero a cinco. Cada thread criada executa um myield, fazendo
 *  com que as threads de prioridades iguais se alternem até acabar. Assim, as
 *  threads de prioridade maior terminam primeiro e as de menor prioridade
 *  terminam por último. Antes de criar as threads, a main executa um
 *  myield, que retorna -1, pois o escalonador ainda não foi inicializado.
 *  Depois que as threads terminaram, o programa executa  myield outra vez, e
 *  o retorno será 0, pois o escalonador já está inicializado. O programa não
 *  recebe nenhum parâmetro e imprime na tela a operação realizada por cada
 *  thread (início, \emph{myield} e término).
 *
 *  usage: primos max_number
 *
 *  saída: imprime na tela os numeros primos pertencentes ao intervalo
 *  [0, max_number)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

int counter = 0;

mmutex_t mutex;

int inc_counter() {
	mlock(&mutex);
	int i = counter++;
	myield();
	munlock(&mutex);
	return i;
}

int is_prime(int n) {
	int i;
	if (n == 0 || n == 1)
		return 0;
	if (n == 2)
		return 1;
	for (i = 2; i <= n / 2; i++) {
		if (n % i == 0)
			return 0;
	}
	return 1;
}

void* print_primes(void* arg) {
	int n = inc_counter();
	if (is_prime(n))
		printf("%d\t", n);
	return NULL;
}

void usage() {
	fprintf(stderr, "usage: primos max_number \n");
	exit(1);
}

int main(int argc, char* argv[]) {
	if (argc != 2)
		usage();
	int i, n_max = atoi(argv[1]);

	int* tids = malloc(sizeof(int) * n_max);

	mmutex_init(&mutex);

	printf("Primes in [0, %d):\n", n_max);
	for (i = 0; i < n_max; i++)
		tids[i] = mcreate(i % 3, (void*(*)(void*)) print_primes, NULL);

	for (i = 0; i < n_max; i++)
		mwait(tids[i]);

	printf("\n");

	return 0;

}

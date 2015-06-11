
/*
 *  Teste mmutex_init
 *
 *  Primeiro é testado se a função é capaz de tratar entradas inválidas
 *  passando como argumento o ponteiro NULL. É esperado que a função retorne
 *  -1, pois não foi possível inicializar tal ponteiro. Depois disso, tenta-se
 *  inicializar um ponteiro válido e o resultado esperado é que a função
 *  retorne 0 e que todos os atributos do mutex estejam zerados.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

int main() {
	mmutex_t mtx;
	printf("Inicializando ponteiro inválido (NULL): retorno %d\n",
				mmutex_init(NULL));
	printf("Inicializando ponteiro válido: retorno %d\n",
				mmutex_init(&mtx));
	printf("Conteúdo do mutex inicializado:\n\tflag: %d\n\tfirst: %p\n\tlast: %p\n",
				mtx.flag, mtx.first, mtx.last);
	return 0;
}

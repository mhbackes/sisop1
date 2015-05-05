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

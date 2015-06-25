 /*
  *
  * Teste mkdir2
  *
  * Este teste é uma versão do comando mkdir do Unix simplificada e adaptada
  * para funcionar com o sistema de arquivos T2FS. Cria um diretório no local
  * do caminho especificado.
  *
  * Uso:
  *  	mkdir2 pathname
  * Onde:
  * pathname é o caminho (absoluto ou relativo ao diretório root) do diretório
  * a ser criado.
  *
  * Saída Esperada:
  * Se o caminho especificado for válido, o programa deverá criar um diretório
  * nesse local no sistema de arquivos T2FS. Se o diretório já existir ou o
  * diretório pai não for encontrado, o programa deve imprimir uma mensagem de erro.
  *
  */

#include <stdio.h>
#include "../include/t2fs.h"

void usage(){
	puts("Usage: mkdir2 pathname");
}

int main (int argc, char* argv[]){
	if(argc != 2){
		usage();
		return 0;
	}
	int error = mkdir2(argv[1]);
	if(error != 0){
		printf("Directory %s created.\n", argv[1]);
	} else {
		puts("Could not create directory.");
	}
}

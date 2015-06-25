 /*
  *
  * Teste rmdir2
  *
  * Este teste é uma versão do comando rmdir do Unix simplificada e adaptada
  * para funcionar com o sistema de arquivos T2FS. Remove o diretório vazio
  * especificado pelo caminho informado.
  *
  * Uso:
  * 	rm2 pathname
  * Onde:
  * pathname é o caminho (absoluto ou relativo ao diretório root) do diretório
  * vazio a ser removido.
  *
  * Saída Esperada:
  * Se o diretório espeficicado for válido o programa remove o diretório. Se o
  * caminho for um diretório não vazio, arquivo ou não existir, o programa
  * deve retornar uma mensagem de erro.
  *
  */

#include <stdio.h>
#include "../include/t2fs.h"

void usage(){
	puts("Usage: rmdir2 pathname");
}

int main (int argc, char* argv[]){
	if(argc != 2){
		usage();
		return 0;
	}
	int error = rmdir2(argv[1]);
	if(error == 0){
		printf("Dirrectory %s removed.\n", argv[1]);
	} else {
		puts("Could not remove directory.");
	}
}

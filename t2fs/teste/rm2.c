 /*
  *
  * Teste rm2
  *
  * Este teste é uma versão do comando rm do Unix simplificada e adaptada para
  * funcionar com o sistema de arquivos T2FS. Remove o arquivo especificado
  * pelo caminho informado.
  *
  * Uso:
  * 	rm2 pathname
  * Onde:
  * filename é o caminho (absoluto ou relativo ao diretório root) do arquivo
  * a ser removido.
  *
  * Saída Esperada:
  * Se o caminho espeficidado for válido, o programa remove o arquivo
  * especificado. Se o caminho for um diretório ou não existir, o programa
  * deve imprimir uma mensagem de erro.
  *
  */

#include <stdio.h>
#include "../include/t2fs.h"

void usage(){
	puts("Usage: rm2 filename");
}

int main (int argc, char* argv[]){
	if(argc != 2){
		usage();
		return 0;
	}
	int error = delete2(argv[1]);
	if(error == 0){
		printf("File %s removed.\n", argv[1]);
	} else {
		puts("Could not remove file.");
	}
	return 0;
}

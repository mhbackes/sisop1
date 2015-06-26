 /*
  * 
  * Teste cat
  * 
  * Este teste é uma versão do comando cat do Unix simplificada e adaptada
  * para funcionar com o sistema de arquivos T2FS. Dado o caminho absoluto do
  * arquivo, o programa imprime seu conteúdo na tela.
  * 
  * Uso:
  * 	cat2 filename
  * Onde:
  * filename é o caminho (absoluto ou relativo ao diretório root) do arquivo
  * armazenado no disco.
  * 
  * Saída Esperada:
  * Se o caminho do arquivo especificado for válido, o programa deve imprimir
  * na tela todo o conteúdo do arquivo. Senão, o programa imprime uma mensagem
  * de erro e termina.
  * 
  */

#include <stdio.h>
#include "../include/t2fs.h"

#define BUFFER_SIZE 1024

void usage() {
	puts("Usage: cat2 filename");
}

int main(int argc, char* argv[]) {
	if(argc != 2){
		usage();
		return 0;
	}

	FILE2 fh = open2(argv[1]);
	if(fh >= 0){
		printf("File %s opened.\n", argv[1]);
	} else {
		puts("Could not open file.");
	}

	char buffer[BUFFER_SIZE];
	int read_bytes = read2(fh, buffer, BUFFER_SIZE);
	while(read_bytes > 0){
		int i;
		for (i = 0; i < read_bytes; i++){
			putchar(buffer[i]);
		}
		read_bytes = read2(fh, buffer, BUFFER_SIZE);
	}

	int error = close2(fh);
	if(error == 0){
		printf("File %s closed.\n", argv[1]);
	} else {
		puts("Could not close file.");
	}
	return 0;
}

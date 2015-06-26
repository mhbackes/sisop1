 /*
  * 
  * Teste cp2
  *
  * Este teste é uma versão do comando cp do Unix simplificada e adaptada para
  * funcionar com o sistema de arquivos T2FS. Dados os caminhos do arquivo de
  * origem no sistema de arquivos atual e o caminho do arquivo de destino a
  * ser criado no sistema de arquivos T2FS, copia o conteúdo do arquivo de
  * origem para o arquivo de destino.
  *
  * Uso:
  * 	cp2 srcfile dstfile
  * Onde:
  * srcfile é o caminho (absoluto ou relativo) do arquivo no sistema de
  * arquivos atual a ser copiado.
  * dstfile é o caminho (absoluto ou relativo ao diretório root) do arquivo no
  * sistema de arquivos T2FS a ser criado.
  *
  * Saída Esperada:
  * Se o arquivo de origem for aberto, o arquivo de destino for criado e a
  * transferência de dados obtiver sucesso, o programa deve imprimir uma
  * mensagem de sucesso. Se alguma dessa operações não for completa, o
  * programa deve imprimir uma mensgem de erro.
  *
  */

#include <stdio.h>
#include "../include/t2fs.h"

#define BUFFER_SIZE 1

void usage() {
	puts("Usage: cp2 srcpath dstpath");
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		usage();
		return 0;
	}

	FILE* srcf = fopen(argv[1], "rb");
	if (srcf != NULL) {
		printf("Source file %s opened.\n", argv[1]);
	} else {
		puts("Could not open source file.");
		return 0;
	}

	FILE2 dstf = create2(argv[2]);
	if (dstf >= 0) {
		printf("Destination file %s created.\n", argv[2]);
	} else {
		printf("Could not create destination file.");
		fclose(srcf);
		return 0;
	}

	char buffer[BUFFER_SIZE];

	puts("Copying file...");
	while (fread(buffer, sizeof(char), BUFFER_SIZE, srcf) != 0) {
		if (write2(dstf, buffer, BUFFER_SIZE) < 0) {
			puts("Could not write bytes in file.");
		}
	}
	puts("File copied.");

	int error = fclose(srcf);
	if (error == 0) {
		printf("Source file %s closed.\n", argv[1]);
	} else {
		puts("Could not close source file.");
	}

	error = close2(dstf);
	if (error == 0) {
		printf("Destination file %s created.\n", argv[2]);
	} else {
		puts("Could not close destination file.");
	}

	return 0;
}

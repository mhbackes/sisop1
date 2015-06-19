#include <stdio.h>
#include "../include/t2fs.h"

int main() {
	char nome[256];
	identify2(nome, 256);
	puts(nome);
}

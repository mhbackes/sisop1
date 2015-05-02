#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

int counter = 0;

mmutex_t* mutex;

int inc_counter() {
	mlock(mutex);
	int i = counter++;
	myield();
	munlock(mutex);
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

	mmutex_init(mutex);	

	printf("Primes in [0, %d):\n", n_max);
	for (i = 0; i < n_max; i++)
		tids[i] = mcreate(1, (void*(*)(void*)) print_primes, NULL);

	for (i = 0; i < n_max; i++)
		mwait(tids[i]);

	printf("\n");

	free(mutex);

	return 0;

}

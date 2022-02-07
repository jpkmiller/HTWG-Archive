#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

#define _MAX 10

int main(void) {

	int *data = malloc(_MAX * sizeof(int));
	sbrk(_MAX);
	free(data);
	for (long long int i = 0; i < _MAX; i++) {
		long long int v = *(data + i);
		if (v != 0) {
			printf("%lld -> %lld\n", i, v);
		}
	}
	for (int i = 0; i < _MAX; i++) {
		
	}
}

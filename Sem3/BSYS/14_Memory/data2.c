#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

#define _MAX 100

int main(void) {

	int *data = calloc(_MAX, sizeof(int));
	free(data);
	for (int i = 0; i < _MAX; i++) {
		int v = *(data + i);
		if (v != 0) {
			printf("%d -> %d\n", i, v);
		}
	}
}

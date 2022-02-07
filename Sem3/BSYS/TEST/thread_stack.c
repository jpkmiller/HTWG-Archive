#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *thread_routine(void *arg) {
	int a;
	printf("addr of thread stack: %p\n", &a);
	return NULL;
}

int main(int argc, char **argv, char **envp) {

	int a;
	printf("addr of main stack: %p\n", &a);

	int max_threads = atoi(argv[1]);
	pthread_t p[max_threads];


	for (int i = 0; i < max_threads; i++) {
		pthread_create(&p[i], NULL, thread_routine, NULL);
	}

	for (int i = 0; i < max_threads; i++) {
		pthread_join(p[i], NULL);
	}

	return 0;
}

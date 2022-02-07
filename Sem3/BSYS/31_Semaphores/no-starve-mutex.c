#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>

typedef struct __nomutex {
	sem_t acquire;
} nomutex;

void init_lock(nomutex *nm) {
	assert(sem_init(&nm->acquire, 0, 1) == 0);
}

void acquire_lock(nomutex *nm) {
	assert(sem_wait(&nm->acquire) == 0);
}

void release_lock(nomutex *nm) {
	assert(sem_post(&nm->acquire) == 0);
}

nomutex nm;
int counter;

void *child(void *arg) {
	acquire_lock(&nm);
	counter++;
	printf("%d\r", counter);
	release_lock(&nm);
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("%s <number of threads>", argv[0]);
	}

	int number_threads = atoi(argv[1]);
	counter = 0;

	pthread_t t[number_threads];

	init_lock(&nm);

	for (int i = 0; i < number_threads; i++) {
		pthread_create(&t[i], NULL, child, (void *) (long long int) i);
	}

	for (int i = 0; i < number_threads; i++) {
		pthread_join(t[i], NULL);
	}

	printf("counter value: %d =? %d\n", counter, number_threads);

	return EXIT_SUCCESS;
}

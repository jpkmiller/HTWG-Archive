#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define __MAX_THREADS 100

sem_t s;
sem_t t;
volatile int threads = 0;

void *child(void *arg) {
	sleep(1);
	printf("child %lld -> %d\n", (long long int) arg, threads);
	sem_wait(&t);
	threads--;
	sem_post(&t);

	if (threads == 0)
		sem_post(&s);

	return NULL;
}

int main(int argc, char *argv[]) {
	sem_init(&s, 0, 0);
	sem_init(&t, 0, 1);
	printf("parent: begin\n");
	pthread_t c[__MAX_THREADS];
	for (int i = 0; i < __MAX_THREADS; i++) {
		sem_wait(&t);
		threads++;
		sem_post(&t)
;
		if (pthread_create(&c[i], NULL, child, (void *) (long long int) i) == -1) {
			perror("thread creation failed\n");
			exit(1);
		}
	}
	sem_wait(&s);
	printf("parent: end\n");
	return 0;
}

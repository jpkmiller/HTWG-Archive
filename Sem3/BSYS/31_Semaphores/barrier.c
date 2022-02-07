#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define __MAX_THREADS 100

/*
sem_wait() decrements (locks) the semaphore pointed to by sem.  If
the semaphore's value is greater than zero, then the decrement
proceeds, and the function returns, immediately.

IMPORTANT:
If the semaphorecurrently has the value zero, then the call blocks until either it
becomes possible to perform the decrement (i.e., the semaphore value
rises above zero), or a signal handler interrupts the call.
*/

typedef struct __barrier_t {
	sem_t s;
	sem_t m;
	int n;
} barrier_t;

void barrier_init(barrier_t *b) {
	sem_init(&b->s, 0, 0);
	sem_init(&b->m, 0, 1);
	sem_wait(&b->m);
	b->n = __MAX_THREADS;
	sem_post(&b->m);
}

void barrier(barrier_t *b) {
	sem_wait(&b->m);
	b->n--;
	sem_post(&b->m);
	if (b->n == 0) {
		sem_post(&b->s);
		return;
	}
	//for checking semaphore
	int val;
	sem_getvalue(&b->s, &val);
	//printf("actual semaphore value is %d\n", val);

	sem_wait(&b->s); //has to wait
	sem_post(&b->s); //when waked up it increments so another thread maybe can wake up
}

void *child(void *arg) {
	barrier_t *b = (barrier_t *) arg;
	printf("started barrier\n");
	barrier(b);
	printf("finished barrier\n");
	return NULL;
}

int main(int argc, char *argv[]) {
	barrier_t *b = (barrier_t *) malloc(sizeof(barrier_t));
	barrier_init(b);

	pthread_t t[__MAX_THREADS];

	printf("parent: begin\n");
	for(int i = 0; i < __MAX_THREADS; i++) {
		pthread_create(&t[i], NULL, child, b);
	}

        for(int i = 0; i < __MAX_THREADS; i++) {
                pthread_join(t[i], NULL);
        }
	printf("parent: end\n");

	return EXIT_SUCCESS;
}

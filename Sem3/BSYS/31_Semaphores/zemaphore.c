#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define __THREAD_AMOUNT 100
#define __THREAD_REPEAT 10000

typedef struct __Zem_t {
	int value;
	pthread_cond_t creation;
	pthread_mutex_t lcreation;
	pthread_cond_t cond;
	pthread_mutex_t lock;
} Zem_t;

typedef struct __Arg_t {
	int tid;
	Zem_t *s;
} Arg_t;


// Init

void Cond_init(pthread_cond_t *cond) {
      	if (pthread_cond_init(cond, NULL) == -1)
		exit(1);
}

void Mutex_init(pthread_mutex_t *lock) {
       	if (pthread_mutex_init(lock, NULL) == -1)
		exit(1);
}

void Arg_init(Arg_t *arg, int tid, Zem_t *s) {
	arg->tid = tid;
	arg->s = s;
	printf("initialized %d -> %d\n", arg->tid, arg->s->value);
}

// Wrapper

void Mutex_lock(pthread_mutex_t *lock) {
	if (pthread_mutex_lock(lock) == -1)
		exit(1);
}

void Mutex_unlock(pthread_mutex_t *lock) {
	if (pthread_mutex_unlock(lock) == -1)
		exit(1);
}

void Cond_wait(pthread_cond_t *cond, pthread_mutex_t *lock) {
	if (pthread_cond_wait(cond, lock) == -1)
		exit(1);
}

void Cond_signal(pthread_cond_t *cond) {
	if (pthread_cond_signal(cond) == -1)
		exit(1);
}

// Semaphore

void Zem_init(Zem_t *s, int value) {
	s->value = value;
	Cond_init(&s->cond);
	Mutex_init(&s->lcreation);
	Cond_init(&s->creation);
	Mutex_init(&s->lock);
}

void Zem_wait(Zem_t *s) {
	Mutex_lock(&s->lock);
	while (s->value <= 0) {
		Cond_wait(&s->cond, &s->lock);
	}
	s->value--;
	Mutex_unlock(&s->lock);
}

void Zem_post(Zem_t *s) {
	Mutex_lock(&s->lock);
	s->value++;
	Cond_signal(&s->cond);
	Mutex_unlock(&s->lock);
}

void *run(void *arg) {
	Cond_wait(&s->
	Arg_t *args = (Arg_t *) arg;
	Zem_t *s = args->s;
	Zem_wait(s);
	for (int i = 0; i < __THREAD_REPEAT; i++) {
		Zem_wait(s);
		printf("%d\n", args->tid);
		Zem_post(s);
	}
}

// main

int main(void) {
	//init semaphore
	Zem_t *s = (Zem_t *) malloc(sizeof(Zem_t));
	Zem_init(s, 10);
	printf("semaphore value is %d\n", s->value);

	//init threads
	pthread_t t[__THREAD_AMOUNT];
	Arg_t arg[__THREAD_AMOUNT];

	for(int i = 0; i < __THREAD_AMOUNT; i++) {
		Arg_init((arg + i), i, s);
		pthread_create((t + i), NULL, run, &arg[i]);
	}

	return EXIT_SUCCESS;
}

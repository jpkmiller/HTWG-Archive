#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define __WRITE_MAX 1000
#define __THREADS_MAX 1000

typedef struct _rwlock_t {
	sem_t writelock; //to block writers (and maybe starve them)
	sem_t readlock; //to block also readers
	sem_t lock;
	int readers;
} rwlock_t;

void rwlock_init(rwlock_t *rw) {
	rw->readers = 0;
	sem_init(&rw->lock, 0, 1);
	sem_init(&rw->writelock, 0, 1);
	sem_init(&rw->readlock, 0, 1);
}

void rwlock_acquire_readlock(rwlock_t *rw) {
	sem_wait(&rw->readlock);
	sem_post(&rw->readlock);
	sem_wait(&rw->lock);
	rw->readers++;
	if(rw->readers == 0)
		sem_wait(&rw->writelock);
	sem_post(&rw->lock);
}

void rwlock_release_readlock(rwlock_t *rw) {
	sem_wait(&rw->lock);
	rw->readers--;
	if (rw->readers == 0)
		sem_post(&rw->writelock);
	sem_post(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
	sem_wait(&rw->lock);
	sem_wait(&rw->readlock);
	sem_wait(&rw->writelock);
	sem_post(&rw->lock);
}

void rwlock_release_writelock(rwlock_t *rw) {
	sem_post(&rw->writelock);
	sem_post(&rw->readlock);
}

rwlock_t mutex;
int writes;

void *reader(void *arg) {
	int local;
	for (int i = 0; i < __WRITE_MAX; i++) {
		rwlock_acquire_readlock(&mutex);
		local = writes;
		printf("reading %d -> %lld\n", writes, (long long int) arg);
		rwlock_release_readlock(&mutex);
	}
	printf("finished reading %d -> %lld\n", local, (long long int) arg);
	return NULL;
}

void *writer(void *arg) {
	int local;
	for (int i = 0; i < __WRITE_MAX; i++) {
                rwlock_acquire_writelock(&mutex);
		local = writes++;
		printf("writing %d -> %lld\n", local, (long long int) arg);
                rwlock_release_writelock(&mutex);
        }
	printf("finished writing %d -> %lld\n", writes, (long long int) arg);
	return NULL;
}

int main(int argc, char *argv[]) {
	rwlock_init(&mutex);
	pthread_t t[__THREADS_MAX];

	for (int i = 0; i < __THREADS_MAX; i++) {
		i % 2 == 0 ? pthread_create(&t[i], NULL, reader, (void *) (long long int) i) : pthread_create(&t[i], NULL, writer, (void *) (long long int) i);
	}

	for (int i = 0; i < __THREADS_MAX; i++) {
		pthread_join(t[i], NULL);
	}

	return EXIT_SUCCESS;
}

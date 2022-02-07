#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t t1;
sem_t t2;

void *person1(void *arg) {
	printf("Man is on the date\n");

	sem_wait(&t2);
	printf("Man waited for the woman\n");
	sem_post(&t1);
	printf("Man goes\n");
	return NULL;
}

void *person2(void *arg) {
	printf("Woman is on the date\n");

	sem_post(&t2);
	printf("Woman waited for the man\n");
	sem_wait(&t1);
	printf("Woman goes\n");
	return NULL;
}

int main(int argc, char *argv[]) {
	sem_init(&t1, 0, 0);
	sem_init(&t2, 0, 0);

	pthread_t p1, p2;
	printf("parent: begin\n");

	if (pthread_create(&p1, NULL, person1, (void *) (long long int) 0) == -1) {
		perror("thread 1 failed");
		exit(1);
	}

	if (pthread_create(&p2, NULL, person2, (void *) (long long int) 1) == -1) {
		perror("thread 2 failed");
		exit(1);
	}

	pthread_join(p1, NULL);
	pthread_join(p2, NULL);

	printf("parent: end\n");

}

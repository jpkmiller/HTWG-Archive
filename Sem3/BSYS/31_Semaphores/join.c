#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

sem_t s;

void *child(void *arg) {
	printf("child\n");
	sleep(10);
	sem_post(&s);
	return NULL;
}

int main(int argc, char *argv[]) {
	sem_init(&s, 0, 0);
	printf("parent: begin\n");
	pthread_t c;
	if (pthread_create(&c, NULL, child, NULL) == -1) {
		perror("thread creation failed\n");
		exit(1);
	}
	sem_wait(&s);
	printf("parent: end\n");
	return 0;
}

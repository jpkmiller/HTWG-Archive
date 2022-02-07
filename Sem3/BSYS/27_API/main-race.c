#include <stdio.h>

#include "mythreads.h"

int balance = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* worker(void* arg) {
    pthread_mutex_t *argLock = (pthread_mutex_t *) arg;
    Pthread_mutex_lock(argLock);
    balance++; // unprotected access
    printf("balance updated\n");
    //Pthread_mutex_unlock(argLock);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    Pthread_create(&p, NULL, worker, &lock);
    Pthread_mutex_lock(&lock);
    balance++; // unprotected access
    Pthread_mutex_unlock(&lock);
    Pthread_join(p, NULL);
    return 0;
}

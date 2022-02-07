#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct __node_t {
	int value;
	struct __node_t *next;
} node_t;

typedef struct __queue_t {
	node_t *head;
	node_t *tail;
	pthread_mutex_t head_lock, tail_lock;
} queue_t;

typedef struct __queue_arg {
    int tid;
	queue_t *q;
} queue_arg;

void Queue_Init(queue_t *q) {
	node_t *tmp = malloc(sizeof(node_t));
	tmp->next = NULL;
	q->head = q->tail = tmp;
	pthread_mutex_init(&q->head_lock, NULL);
	pthread_mutex_init(&q->tail_lock, NULL);
}

void Queue_Enqueue(queue_t *q, int value) {
	node_t *tmp = malloc(sizeof(node_t));
	assert(tmp != NULL);
	tmp->value = value;
	tmp->next = NULL;

	pthread_mutex_lock(&q->tail_lock);
	q->tail->next = tmp;
	q->tail = tmp;
	pthread_mutex_unlock(&q->tail_lock);
}

int Queue_Dequeue(queue_t *q, int *value) {
	pthread_mutex_lock(&q->head_lock);
	node_t *tmp = q->head;
	node_t *new_head = tmp->next;
	if (new_head == NULL) {
		pthread_mutex_unlock(&q->head_lock);
		return -1; //queue was empty
	}
	*value = new_head->value;
	q->head = new_head;
	pthread_mutex_unlock(&q->head_lock);
	free(tmp);
	return 0;
}

void *Thread_Queue(void *arg) {
	queue_arg *args = (queue_arg *) arg;
    for(int i;; i++) {
        Queue_Enqueue(args->q, i);
        if (i % 20 == 0) {
            printf("%d put %d\n", args->tid, i);
            pthread_yield();
        }
	}
}

int main(void) {
	pthread_t p1, p2, p3, p4;

	queue_t *q = (queue_t *) malloc(sizeof(queue_t));
	q->head = (node_t *) malloc(sizeof(node_t));
	q->tail = (node_t *) malloc(sizeof(node_t));
	Queue_Init(q);
    int *value = (int *) malloc(sizeof(int));
    
    printf("initializing queue\n");

    queue_arg q1 = {0, q};
    queue_arg q2 = {1, q};
    queue_arg q3 = {2, q};
    queue_arg q4 = {3, q};

    printf("creating threads\n");

	pthread_create(&p1, NULL, Thread_Queue, &q1);
	pthread_create(&p2, NULL, Thread_Queue, &q2);
	pthread_create(&p3, NULL, Thread_Queue, &q3);
	pthread_create(&p4, NULL, Thread_Queue, &q4);

    for (int i = 0;; i++) {
        Queue_Dequeue(q, value);
        if (i % 1 == 0) {
            printf("received %d\n", *value);
        }
    }
    
    printf("threads created\n");

	return 0;
}

#include "myhead.h"

typedef struct{
	int *arr;
	int len;
}task_t;


typedef struct{
	task_t *buf;
	int head, tail;
	int n;//should be 2^k
	pthread_mutex_t mutex;
	//sem_t slots;
	sem_t items;
}sbuf_t;


int nlpo2(int x){//next largest power of 2
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x + 1;
}
void sbuf_init(sbuf_t *sp, int n){
	int nlp = nlpo2(n);
	sp->buf = (task_t *)malloc(nlp * sizeof(task_t));
	sp->n = nlp;
	sp->head = 0;
	sp->tail = 0;
	pthread_mutex_init(&sp->mutex, NULL);
	//sem_init(&sp->slots, 0, nlp);
	sem_init(&sp->items, 0, 0);
}

void sbuf_deinit(sbuf_t *sp){
	free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, task_t task){
	//sem_wait(&sp->slots);
	pthread_mutex_lock(&sp->mutex);

	sp->buf[(++sp->tail) & (sp->n - 1)] = task;//& is actually % because n = 2^k

	pthread_mutex_unlock(&sp->mutex);
	sem_post(&sp->items);
}

task_t  sbuf_remove(sbuf_t *sp){
	task_t task;
	sem_wait(&sp->items);
	pthread_mutex_lock(&sp->mutex);

	task = sp->buf[(++sp->head) & (sp->n - 1)];//& is actually % because n = 2^k

	pthread_mutex_unlock(&sp->mutex);
	//sem_post(&sp->slots);

	return task;
}

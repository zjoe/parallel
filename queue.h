#include "myhead.h"
#define new_node() (NODE *)malloc(sizeof(NODE))

typedef struct node_t{
	struct node_t *next;
	int *arr;
	int len;
}NODE;

typedef struct queue_t{
	NODE *head; 
	char padding[128];
	NODE *tail;
	pthread_mutex_t qh_lock, qt_lock;
	sem_t items;
}Q;

void queue_init(Q *q){
	NODE *node = new_node();
	node->next = NULL;
	q->head = q->tail = node;
	pthread_mutex_init(&q->qh_lock, NULL);
	pthread_mutex_init(&q->qt_lock, NULL);
	sem_init(&q->items, 0, 0);
}

void enqueue(Q *q, NODE *node){
	pthread_mutex_lock(&q->qt_lock);

	q->tail->next = node;//this operation is atomic
	q->tail = node;

	pthread_mutex_unlock(&q->qt_lock);
	sem_post(&q->items);
}

void dequeue(Q *q, NODE *node){
	NODE *temp;
	sem_wait(&q->items);
	pthread_mutex_lock(&q->qh_lock);

	temp = q->head;
	q->head = temp->next;
	*node = *(temp->next);

	pthread_mutex_unlock(&q->qh_lock);
	
	free(temp);
}

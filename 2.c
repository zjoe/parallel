//#include "sbuf.h"
#include "queue.h"

#define MAXCPUNUM 100

int N = 1000000, rest, cpuNum = 2;
int *array;
//sbuf_t tline;
Q tline;
pthread_t tid[MAXCPUNUM];
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void work();
void *thread(void *vargp);
void qdivide(NODE *t, NODE *tl, NODE *tr);
void shellsort(NODE *t);
int main(const int argc, const char *argv[]){
	N = atoi(argv[1]);
	rest = N;
	cpuNum = atoi(argv[2]);

	/*create the array*/
	array = (int *)malloc(N * sizeof(int));
	srand((unsigned)time(0));
	int i;
	for(i = 0; i < N; i++){
		array[i] = rand() % (10 * N);
	}
	
	/*init the task line*/
	queue_init(&tline);

	timeCounting(work);
	return 0;
}
void work(){
	int i;
	
	/*create the first task*/
	NODE *task = new_node();
	task->arr = array;
	task->len = N;
	enqueue(&tline, task);

	/*create threads*/
	//cpuNum = get_nprocs();
	printf("cpuNum = %d\n", cpuNum);
	for(i = 0; i < cpuNum; i++){
		pthread_create(&tid[i], NULL, thread, NULL);
	}
	
	/*wait for finish-signal*/
	pthread_mutex_lock(&mtx);
	pthread_cond_wait(&cond, &mtx);
	for(i = 0; i < cpuNum; i++){
		pthread_cancel(tid[i]);
	}
	pthread_mutex_unlock(&mtx);

	/*waiting for threads to end*/
	for(i = 0; i < cpuNum; i++){
		pthread_join(tid[i], NULL);
	}

	/*free task line space*/
	//sbuf_deinit(&tline);

	//for(i = 0; i < N; i++)
	//	printf("%d ", array[i]);
	//printf("\n");
}
void *thread(void *vargp){
	NODE t, *taskl, *taskr;
	int finished, jobleft = 0;

	while(1){

		dequeue(&tline, &t);
		jobleft = 1;
		finished = 0;

		while(jobleft && t.len > 650){
			//printf("[%lu] get task: beg = %d len = %d\n", pthread_self(), t.arr - array, t.len);

			//input t, output taskl and taskr; use pointer to t for efficiency
			taskl = new_node();
			taskr = new_node();
			qdivide(&t, taskl, taskr);	

			/*counting finished and push new tasks*/
			finished++;
			jobleft = 0;
			if(taskl->len > 1){
				jobleft = 1;
				t = *taskl;
			}
			else{
				finished += taskl->len;
			}
				
			if(taskr->len > 1){
				if(!jobleft){
					jobleft = 1;
					t = *taskr;
				}
				else{
					enqueue(&tline, taskr);
				}
			}
			else{
				finished += taskr->len;
			}
		}
		if(jobleft){
			shellsort(&t);
			finished += t.len;
		}


		/*wake main thread when all tasks finished*/
		pthread_mutex_lock(&mtx);
		rest -= finished;
		if(rest == 0){
			pthread_cond_signal(&cond);
		}
		pthread_mutex_unlock(&mtx);

	}
}
void qdivide(NODE *t, NODE *tl, NODE *tr){
	int *a = t->arr;
	int len = t->len;
	int randpos = rand() % len;
	int temp = a[0];
	int l = 0, r = len - 1;
	a[0] = a[randpos];
	a[randpos] = temp;
	temp = a[0];
	while(l < r){
		while(l < r && a[r] >= temp){
			r--;
		}
		if(l < r){
			a[l] = a[r];
			l++;
		}
		
		while(l < r && a[l] <= temp){
			l++;
		}
		if(l < r){
			a[r] = a[l];
			r--;
		}
	}
	a[l] = temp;
	
	tl->arr = a;
	tl->len = l;
	tr->arr = a + l + 1;
	tr->len = len - 1 - l;
}
void shellsort(NODE *t){
	int d, temp, i, j;
	int *arr = t->arr;
	d = t->len >> 1;
	while(d > 0){
		for(i = d; i < t->len; i++){
			temp = arr[i];
			j = i - d;
			while(j >= 0 && temp < arr[j]){
				arr[j + d] = arr[j];
				j -= d;
			}
			arr[j + d] = temp;
		}
		d >>= 1;
	}
}

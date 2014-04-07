#include "myhead.h"

int N;
int *array;

int cmp(const void *a, const void *b);
void work();

int main(const int argc, const char *argv[]){
	N = atoi(argv[1]);

	array = (int *)malloc(N * sizeof(int));
	int i;
	srand((unsigned)time(0));
	for(i = 0; i < N; i++)
		array[i] = rand() % (10 * N);

	timeCounting(work);

	return 0;
}
void work(){
	qsort((void *)array, N, sizeof(int), cmp);
}
int cmp(const void *a, const void *b){
	return *(int*)a - *(int*)b;
}

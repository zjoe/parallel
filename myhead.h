#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>

void timeCounting(void (*func)()){
	struct timeval start, end;
	gettimeofday(&start, NULL);
	func();
	gettimeofday(&end, NULL);
	printf("%lf\n", end.tv_sec - start.tv_sec + (double)(end.tv_usec - start.tv_usec) / 1000000);
}

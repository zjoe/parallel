#include "myhead.h"
int N = 100000;
void work();
int main(const int argc, const char *argv[]){
	N = atoi(argv[1]);
	timeCounting(work);
	return 0;
}
void work(){
	double ans = 0;
	int i;
	for(i = 1; i <= N; i++){
		ans += sqrt(i) / (i + 1);
	}
	printf("ans = %lf\n", ans);
}

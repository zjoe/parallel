#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int glob = 6;
pid_t createChild(){
	pid_t pid = fork();

	if(pid == 0){
		printf("child do something here\n");
		exit(0);
	}
	return pid;
}
int main(void)
{
	pid_t pid;
	int i, procNum = 3;

	for(i = 0; i < procNum; i++){
		pid = createChild();
	}
	return 0;
}

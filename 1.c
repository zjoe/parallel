#include "myhead.h"

#define MY_MQ_P2C_NAME "/mymqP2C"
#define MY_MQ_C2P_NAME "/mqmqC2P"
#define MY_MQ_MAXMSG 10
#define MY_MQ_MSGSIZE 64

int N = 100000;
char msg_buff[MY_MQ_MSGSIZE];

void work();
pid_t createChild(mqd_t r_mqdes, mqd_t s_mqdes);

int main(const int argc, const char *argv[]){
	N = atoi(argv[1]);
	timeCounting(work);
	return 0;
}

void work(){
	pid_t childPid;
	int cpuNum = 1, waitStatus, i, exitMsg = -1;
	clock_t beg, end;

	/*parent open message queue*/
	mq_unlink(MY_MQ_P2C_NAME);
	mq_unlink(MY_MQ_C2P_NAME);//unlink first
	struct mq_attr attr;
	attr.mq_maxmsg = MY_MQ_MAXMSG;
	attr.mq_msgsize = MY_MQ_MSGSIZE;
	attr.mq_flags = 0;//blocked

	mqd_t p2cmqdes = mq_open(MY_MQ_P2C_NAME, O_RDWR | O_CREAT, 0664, &attr);//post
	if(p2cmqdes < 0){
		printf("parent(%d): %s\n", getpid(), strerror(errno));
		exit(-1);
	}

	mqd_t c2pmqdes = mq_open(MY_MQ_C2P_NAME, O_RDWR | O_CREAT, 0664, &attr);//receive
	if(c2pmqdes < 0){
		printf("parent(%d): %s\n", getpid(), strerror(errno));
		exit(-1);
	}

	/*create child proccess*/
	cpuNum = get_nprocs();
	printf("cpuNum = %d\n", cpuNum);
	for(i = 0; i< cpuNum; i++){
		childPid = createChild(p2cmqdes, c2pmqdes);
	}

	/*send tasks*/
	for(i = 1; i <= N; i++){
		memcpy(msg_buff, &i, sizeof(int));
		mq_send(p2cmqdes, msg_buff, sizeof(int), 0);
	}

	/*tell children to exit*/
	for(i = 0; i < cpuNum; i++){
		memcpy(msg_buff, &exitMsg, sizeof(int));
		mq_send(p2cmqdes, msg_buff, sizeof(int), 0);
	}

	/*receive results from children*/
	double total = 0;
	for(i = 0; i < cpuNum; i++){
		uint prio;
		mq_receive(c2pmqdes, msg_buff, MY_MQ_MSGSIZE, &prio);
		double sum;
		memcpy(&sum, msg_buff, sizeof(double));
		memcpy(&childPid, msg_buff + sizeof(double), sizeof(int));
		printf("received from child(%d): %f\n", childPid, sum);
		total += sum;
	}
	printf("ans = %f\n", total);

	/*wait all children to exit*/
	while( (childPid = wait(&waitStatus)) > 0 ){
		printf("child %d exit\n", childPid);
	}

	mq_close(p2cmqdes);
	mq_close(c2pmqdes);

	mq_unlink(MY_MQ_C2P_NAME);
	mq_unlink(MY_MQ_P2C_NAME);

}

pid_t createChild(mqd_t r_mqdes, mqd_t s_mqdes){
	int recvValue;
	uint prio;
	pid_t pid = fork();

	double sum = 0;
	if(pid == 0){
		while(1){
			ssize_t msgSize = mq_receive(r_mqdes, msg_buff, MY_MQ_MSGSIZE, &prio);
			if(msgSize < 0){
				printf("child(%d):receive error: %s\n", getpid(), strerror(errno));
				exit(1);
			}
			memcpy(&recvValue, msg_buff, msgSize);

			if(recvValue < 0)break;

			sum += sqrt(recvValue) / (recvValue + 1);

		}
		/*send result to the parent*/
		memcpy(msg_buff, &sum, sizeof(double));
		pid_t childPid = getpid();
		memcpy(msg_buff + sizeof(double), &childPid, sizeof(pid_t));
		mq_send(s_mqdes, msg_buff, sizeof(double) + sizeof(pid_t), 1);

		exit(0);
	}
	return pid;
}

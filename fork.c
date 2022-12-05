#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sem_com.c"
#define DELAY_TIME 3 //为了突出演示效果，等待几秒钟
int main(void)
{
	pid_t result;
	int sem_id;  //定义信号量集的ID号
	sem_id = semget(ftok("/",1),1,0666|IPC_CREAT);
	init_sem(sem_id,0,0); //初始化信号量，因为为同步问题，所以初始值设置为0
	//调用fork函数创建子进程
	result = fork();
	if(result == -1)
	{
		perror("Fork\n");
	}
	else if(result == 0)  //返回值为0代表子进程，在子进程中直接运行程序，最后调用V操作释放资源
	{
		printf("Child process will wait for some seconds...\n");
		sleep(DELAY_TIME);
		printf("The returned value is %d,in the child process(PID = %d)\n",result,getpid());
		sem_v(sem_id,0);
	}
	else
	{
		sem_p(sem_id,0);
		printf("The returned value is %d,in the father process(PID = %d)\n",result,getpid());
		del_sem(sem_id);
	}
	exit(0);

}

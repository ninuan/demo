#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sem_com.c"
#define SRC_FILE_NAME "src_file" //源文件名
#define DEST_FILE_NAME "dest_file" //目标文件名
#define OFFSET 1

int src_file,tmp_file;
char consuming()
{
	char item;
	char buff;
	int real_read_len;
	src_file = open(SRC_FILE_NAME, O_RDWR);
	tmp_file = open(DEST_FILE_NAME, O_RDWR|O_CREAT|O_TRUNC,666);
	if(src_file < 0)
	{
		printf("open file error\n");
		exit(1);
	}
	if(tmp_file < 0)
	{
		printf("tmp file error\n");
		exit(1);
	}
	read(src_file,&item,sizeof(char));
	while((real_read_len = read(src_file,&buff,sizeof(buff)))>0)
	{
		write(tmp_file,&buff,real_read_len);
	}
	close(src_file);
	close(tmp_file);

	src_file = open(SRC_FILE_NAME,O_RDWR|O_TRUNC);
	tmp_file = open(DEST_FILE_NAME,O_RDWR);
	while((real_read_len = read(tmp_file,&buff,sizeof(buff)))>0)
	{
		write(src_file,&buff,real_read_len);
	}
	close(src_file);
	close(tmp_file);
	return item;
}

int main()
{
	pid_t result;
	int empty,full,mutex,mutex1; //定义信号量集的ID号
	//如果对应该键值到信号量集已经存在，就删除重新创建一个信号量集，以防止多次运行导致的信号量紊乱
	if(full = semget(ftok("/",1),1,0666))
	{
		del_sem(full);
		full = semget(ftok("/",1),1,0666|IPC_CREAT);
	}
	else
	{
		full = semget(ftok("/",1),1,0666|IPC_CREAT);
	}
	//***************************************************
	if(empty = semget(ftok("/",2),1,0666))
	{
		del_sem(empty);
		empty = semget(ftok("/",2),1,0666|IPC_CREAT);
	}
	else
	{
		empty = semget(ftok("/",2),1,0666|IPC_CREAT);
	}
	//***************************************************
	if(mutex = semget(ftok("/",3),1,0666))
	{
		del_sem(mutex);
		mutex = semget(ftok("/",3),1,0666|IPC_CREAT);
	}
	else
	{
		mutex = semget(ftok("/",3),1,0666|IPC_CREAT);
	}
	//***************************************************
	if(mutex1 = semget(ftok("/",4),1,0666))
	{
		del_sem(mutex1);
		mutex1 = semget(ftok("/",4),1,0666|IPC_CREAT);
	}
	else
	{
		mutex1 = semget(ftok("/",4),1,0666|IPC_CREAT);
	}

	//初始化信号量
	init_sem(full,0,0);
	init_sem(empty,0,0);
	init_sem(mutex,0,1);
	init_sem(mutex1,0,1); //为了让生产者和消费者中各自的三句话不分开

	//调用fork函数创建子进程
	result = fork();
	if(result == -1)
	{
		perror("Fork\n");
	}
	else if(result == 0) //返回值为0代表子进程，这里用作生产者进程
	{
		src_file = open(SRC_FILE_NAME,O_RDWR|O_CREAT|O_APPEND,666);
		while(1)
		{
			char i = '0';
			while(i <= '9')
			{
				if(semctl(empty,0,GETVAL,NULL) > 0)
				{
					sem_p(mutex1,0);
					printf("===Before produce:full %d empty %d===\n\n",semctl(full,0,GETVAL,NULL),semctl(empty,0,GETVAL,NULL));
					sem_p(empty,0);
					sem_p(mutex,0);
					write(src_file,&i,sizeof(i));
					printf("Producer:the produced item is %c \n\n",i);
					sem_v(mutex,0);
					sem_v(full,0);
					printf("===After produce:full %d empty %d===\n\n",semctl(full,0,GETVAL,NULL),semctl(empty,0,GETVAL,NULL));
					sem_v(mutex1,0);
					i = i + 1;
				}
				else
				{
					printf("=========================\n");
					printf("Full\n\n");
				}
				sleep(1);
			}
		}
	}
	else  //返回值大于0代表父进程，这里用作消费者进程
	{
		char item;
		while(1)
		{
			if(semctl(full,0,GETVAL,NULL) > 0)
			{
				sem_p(mutex1,0);
				printf("---Before consume:full %d empty %d---\n\n",semctl(full,0,GETVAL,NULL),semctl(empty,0,GETVAL,NULL));
				sem_p(full,0);
				sem_p(mutex,0);
				item = consuming();
				printf("Consumer:the consumed item is %c \n\n",item);
				sem_v(mutex,0);
				sem_v(empty,0);
				printf("---After consumer:full %d empty %d---\n\n",semctl(full,0,GETVAL,NULL),semctl(empty,0,GETVAL,NULL));
				sem_v(mutex1,0);
			}
			else
			{
				printf("================================\n");
				printf("empty\n\n");
			}
			sleep(2);
		}
	}
}

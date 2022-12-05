//信号量初始化（赋值）函数
int init_sem(int sem_id, int n, int init_value)
{
	union semun
	{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	};
	union semun sem_union;
	sem_union.val = init_value;    //init_value 为初始值
	if(semctl(sem_id,n,SETVAL,sem_union) == -1)
	{
		perror("Initialize semaphore");
		return -1;
	}
	return 0;
}


//从系统中删除信号量集的函数
int del_sem(int sem_id)
{
	if(semctl(sem_id,0,IPC_RMID,NULL) == -1)
	{
		perror("Delete semaphore");
		return -1;
	}
	return 0;
}


//P操作函数
int sem_p(int sem_id, int n)
{
	struct sembuf sem_b;
	sem_b.sem_num = n;  //单个信号量的编号如果为0，即信号量集中的第一个信号量
	sem_b.sem_op = -1;  //表示P操作
	sem_b.sem_flg = SEM_UNDO;  //系统会自动释放将会在系统中残留的信号量
	if(semop(sem_id, &sem_b, 1) == -1)
	{
		perror("P operation");
		return -1;
	}
	return 0;
}


//V操作函数
int sem_v(int sem_id, int n)
{
	struct sembuf sem_b;
	sem_b.sem_num = n;  //单个信号量的编号如果为0，即信号量集中的第一个信号量
	sem_b.sem_op = 1;  //表示V操作
	sem_b.sem_flg = SEM_UNDO;  //系统会自动释放将会在系统中残留的信号量
	if(semop(sem_id, &sem_b, 1) == -1)
	{
		perror("V operation");
		return -1;
       	}
	return 0;

}






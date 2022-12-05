#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(){
	
	int x,y;
	x = 0x1010002;
	printf("x = %p\n",x);
	y = semget(x,1,0666|IPC_CREAT);
	if(y<0){
		perror("semget error");
	}
	printf("y = %d\n",y);
	
}

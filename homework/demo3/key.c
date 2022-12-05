#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(){
	int x;
	x = ftok("/",1);  //在根目录下创建一个ID为1的键值
	printf("x = %d\n",x);

}

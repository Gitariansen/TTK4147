#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

int running = 1;
int var1 = 0;
int var2 = 0;


pthread_mutex_t lock;

void *thread1(){
	while(running){
		pthread_mutex_lock(&lock);
		var1 = var1 + 1;
		var2 = var1;
		pthread_mutex_unlock(&lock);
	}
}

void *thread2(){
	int i;
	for(i = 0; i < 25; i++)
	{
		pthread_mutex_lock(&lock);
		printf("Number 1: %d \nNumber 2: %d \n", var1, var2);
		pthread_mutex_unlock(&lock);
		sleep(1);
	}
	running = 0;
}


int main(){


	pthread_t pthread1;
	pthread_t pthread2;

	
	pthread_create(&pthread1, NULL, thread1, NULL);
	pthread_create(&pthread2, NULL, thread2, NULL);

	pthread_join(pthread1, NULL);
	pthread_join(pthread2, NULL);
	
	return 0;
}

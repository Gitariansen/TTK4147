#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int globalInt = 0;

void *thread(){
	int localInt = 0;
	int i;
	
	for(i = 0; i < 5; i++)
	{
		localInt++;
		globalInt++;
	} 
	
	printf("From thread:\n Global = %d \n Local = %d \n", globalInt, localInt);
}

int main(){
	pthread_t pthread1;
	pthread_t pthread2;
	
	pthread_create(&pthread1, NULL, thread, NULL);
	pthread_create(&pthread2, NULL, thread, NULL);
	
	
	pthread_join(pthread1, NULL);
	pthread_join(pthread2, NULL);
	
	return 0;
}

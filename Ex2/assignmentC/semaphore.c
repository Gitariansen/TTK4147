#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

sem_t sem;



void *thread(){
	sem_wait(&sem);
	pid_t x = syscall(__NR_gettid);
	printf("starting using resources %d \n", x);
	int i;
	for(i=0; i < 4; i++){
		printf("wait %d \n", i);
		sleep(1);
	}
	printf("Done using resource\n");
	sem_post(&sem);
}


int main(){
	sem_init(&sem, 0, 3);

	pthread_t pthread1;
	pthread_t pthread2;
	pthread_t pthread3;
	pthread_t pthread4;
	pthread_t pthread5;
	
	pthread_create(&pthread1, NULL, thread, NULL);
	pthread_create(&pthread2, NULL, thread, NULL);
	pthread_create(&pthread3, NULL, thread, NULL);
	pthread_create(&pthread4, NULL, thread, NULL);
	pthread_create(&pthread5, NULL, thread, NULL);

	pthread_join(pthread1, NULL);
	pthread_join(pthread2, NULL);
	pthread_join(pthread3, NULL);
	pthread_join(pthread4, NULL);
	pthread_join(pthread5, NULL);
	
	return 0;
}


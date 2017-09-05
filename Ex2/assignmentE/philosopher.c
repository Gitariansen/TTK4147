#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>


typedef struct {
	int position;
	int count;
	pthread_mutex_t *forks;
} params_t;

pthread_mutex_t folkeskikk;

void *philosopher(void *params);
void think(int position);
void eat(int position);

void run_all_threads(pthread_t *threads, pthread_mutex_t *forks, int num_philosophers);


int main(int argc, char *args[])
{
	int num_philosophers = 5;
	
	//static pthread_mutex_t forks[5];
	static pthread_mutex_t * forks;
	forks = malloc(num_philosophers*sizeof(pthread_mutex_t));
	
	pthread_t philosophers[num_philosophers];
	
	run_all_threads(philosophers, forks, num_philosophers);

	pthread_exit(NULL);
	
	return 0;
}

void run_all_threads(pthread_t *threads, pthread_mutex_t *forks, int num_philosophers)
{
	int i;
	for(i = 0; i < num_philosophers; i++) {
		params_t *arg = malloc(sizeof(params_t));
		arg->position = i;
		arg->count = num_philosophers;
		arg->forks = forks;
		pthread_create(&threads[i], NULL, philosopher, (void *)arg);
	}
}


void *philosopher(void *params)
{

	int i;
	params_t self = *(params_t *)params;
	for(i = 0; i < 3; i++) {
		think(self.position);
		pthread_mutex_lock(&folkeskikk);
		printf("philosopher %d locked the folkeskikk \n", self.position);
		pthread_mutex_lock(&self.forks[self.position]);
		pthread_mutex_lock(&self.forks[(self.position+1)%self.count]);
		eat(self.position);
		pthread_mutex_unlock(&self.forks[(self.position+1)%self.count]);
		pthread_mutex_unlock(&self.forks[self.position]);
		pthread_mutex_unlock(&folkeskikk);
		printf("philosopher %d unlocked the folkeskikk \n", self.position);
		
	}
	think(self.position);
	pthread_exit(NULL);
}


void think(int position)
{
	printf("Philosopher %d is thinking\n", position);
	sleep(1);
}
void eat(int position)
{
	printf("Philosopher %d is eating\n", position);
	sleep(1);
}








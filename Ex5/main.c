//need these?
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>


#include "io.h"


void *disturbance_func(){
	int i = 0;
	while(1)
	{
		i += 10;
	}
}

void *test_pin(void * i);
void *periodic_test_pin(void * i);

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void timespec_add_us(struct timespec *t, long us)
{
	// add microseconds to timespecs nanosecond counter
	t->tv_nsec += us*1000;

	// if wrapping nanosecond counter, increment second counter
	if (t->tv_nsec > 1000000000){
		t->tv_nsec = t->tv_nsec-1000000000;
		t->tv_sec += 1;
	}
}

int main(){
	io_init();

	/*int toggle = 0;
	while(1){
		io_write(1,toggle);
		toggle = (toggle) ? 0 : 1;	
		usleep(1000 * 500);	
	}
	*/

	pthread_t testA;
	pthread_t testB;
	pthread_t testC;
	
	pthread_t d1;
	pthread_t d2;
	pthread_t d3;
	pthread_t d4;
	pthread_t d5;
	pthread_t d6;
	pthread_t d7;
	pthread_t d8;
	pthread_t d9;
	pthread_t d10;
	
	int i1 = 1;
	int i2 = 2;
	int i3 = 3;
/*
	pthread_create(&testA, NULL, test_pin, &i1);
	pthread_create(&testB, NULL, test_pin, &i2);
	pthread_create(&testC, NULL, test_pin, &i3);
*/

	pthread_create(&testA, NULL, periodic_test_pin, &i1);
	pthread_create(&testB, NULL, periodic_test_pin, &i2);
	pthread_create(&testC, NULL, periodic_test_pin, &i3);




	pthread_create(&d1, NULL, disturbance_func, NULL);
	pthread_create(&d2, NULL, disturbance_func, NULL);
	pthread_create(&d3, NULL, disturbance_func, NULL);
	pthread_create(&d3, NULL, disturbance_func, NULL);
	pthread_create(&d4, NULL, disturbance_func, NULL);
	pthread_create(&d5, NULL, disturbance_func, NULL);
	pthread_create(&d6, NULL, disturbance_func, NULL);
	pthread_create(&d7, NULL, disturbance_func, NULL);
	pthread_create(&d8, NULL, disturbance_func, NULL);
	pthread_create(&d9, NULL, disturbance_func, NULL);
	pthread_create(&d10, NULL, disturbance_func, NULL);

	pthread_join(testA, NULL);
	pthread_join(testB, NULL);
	pthread_join(testC, NULL);
	pthread_join(d1, NULL);
	pthread_join(d2, NULL);
	pthread_join(d3, NULL);
	pthread_join(d4, NULL);
	pthread_join(d5, NULL);
	pthread_join(d6, NULL);
	pthread_join(d7, NULL);
	pthread_join(d8, NULL);
	pthread_join(d9, NULL);
	pthread_join(d10, NULL);



	return 0;
}
void *periodic_test_pin(void* i){
	set_cpu(1);
	int x = *((int*)i);

	struct timespec next;

	clock_gettime(CLOCK_REALTIME, &next);

	while(1){
		timespec_add_us(&next, 1000);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);

		if(io_read(x)==0){
			io_write(x, 0);
			timespec_add_us(&next, 5);
			clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
			io_write(x, 1);
		}
		
		//printf("hello\n");
	
	}

}
void *test_pin(void* i){
	set_cpu(1);
	int x = *((int*)i);
	while(1){
		if(io_read(x)==0){
			io_write(x, 0);
			usleep(5);
			io_write(x, 1);
		}
	}
}







		

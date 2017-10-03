//need these?
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

#include <sys/mman.h>

#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

#include "io.h"

RT_TASK task_A;
RT_TASK task_B;
RT_TASK task_C;

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void *disturbance_func(){
	set_cpu(1);
	int i = 0;

	double dummy;
	while(1)
	{
		for(i=0;i< 10000000; i++)
		{
			dummy = 2.5* i;
		}	
	}
}


void periodic_test_pin(void * i);



int main(){
	
	mlockall(MCL_CURRENT | MCL_FUTURE);

	io_init();
	rt_print_auto_init(1);

	io_write( 1, 1 );
	io_write( 2, 1 );
	io_write( 3, 1 );
	

	// CREATING DISTURBANCE THREADS //

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

	 /* Create task
   * Arguments: &task,
   *            name,
   *            stack size (0=default),
   *            priority,
   *            mode (FPU, start suspended, ...)
   */
	rt_task_create(&task_A, "A", 0, 99, (T_CPU(1) | T_JOINABLE) );
	rt_task_create(&task_B, "B", 0, 99, (T_CPU(1) | T_JOINABLE) );
	rt_task_create(&task_C, "C", 0, 99, (T_CPU(1) | T_JOINABLE) );


	/*  Start task
   * Arguments: &task,
   *            task function,
   *            function argument
   */
	
	int i1 = 1;
	int i2 = 2;
	int i3 = 3;
	rt_task_start(&task_A, &periodic_test_pin, &i1);
	rt_task_start(&task_B, &periodic_test_pin, &i2);
	rt_task_start(&task_C, &periodic_test_pin, &i3);


	rt_task_join(&task_A);
	rt_task_join(&task_B);
	rt_task_join(&task_C);

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

void periodic_test_pin(void* i){
	set_cpu(1);

	rt_task_set_periodic(NULL, TM_NOW, 1000*50);
	int x = *((int*)i);

	while(1){

		if(io_read(x)==0){
			io_write(x, 0);
			rt_task_sleep(1000*5);
			io_write(x, 1);
		}
		rt_task_wait_period(NULL);
		
		
	
	}

}









		

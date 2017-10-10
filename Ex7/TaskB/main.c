#include <stdio.h>
#include <unistd.h>
#include <rtdk.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

//Xenomai stuff
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>

// Semaphores
RT_SEM sem;
RT_MUTEX mutex;

// Tasks
RT_TASK sync_task;
RT_TASK high_priority_task;
RT_TASK med_priority_task;
RT_TASK low_priority_task;

#define number_of_tasks 2
//#define use_mutex true

void low_task();
void med_task();
void high_task();
void sync_func();
void delete_all_sems();
void busy_wait_ms(unsigned long delay);

int main(){
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);

	// create tasks
	rt_task_create(&sync_task, "sync", 0, 4, (T_CPU(1) | T_JOINABLE));
	rt_task_create(&high_priority_task, "high", 0, 3, (T_CPU(1) | T_JOINABLE));
	rt_task_create(&med_priority_task, "med", 0, 2, (T_CPU(1) | T_JOINABLE));
	rt_task_create(&low_priority_task, "low", 0, 1, (T_CPU(1) | T_JOINABLE));
	// create semaphore
	rt_sem_create(&sem, "sem", 0, S_FIFO);
	rt_mutex_create(&mutex, "elsa_sin_muti");

	// start tasks
	rt_task_start(&low_priority_task, &low_task, NULL);
	rt_task_start(&med_priority_task, &med_task, NULL);
	rt_task_start(&high_priority_task, &high_task, NULL);
	rt_task_start(&sync_task, &sync_func, NULL);

	//join all tasks
	rt_task_join(&sync_task);
	rt_task_join(&high_priority_task);
	rt_task_join(&med_priority_task);
	rt_task_join(&low_priority_task);

	delete_all_sems();
	return 0;
}


void low_task(){
#ifdef use_mutex
	rt_mutex_acquire(&mutex, TM_INFINITE);
#else
	rt_sem_p(&sem, TM_INFINITE);
#endif

	busy_wait_ms(300); //Do work
	rt_printf("Low task complete\n");

#ifdef use_mutex
	rt_mutex_release(&mutex);
#else
	rt_sem_v(&sem);
#endif
}

void med_task(){
	rt_task_sleep(1000*1000*100);
	busy_wait_ms(500); //Do work
	rt_printf("Medium task complete\n");
}

void high_task(){
	rt_task_sleep(1000*1000*200);

#ifdef use_mutex
	rt_mutex_acquire(&mutex, TM_INFINITE);
#else
	rt_sem_p(&sem, TM_INFINITE);
#endif

	busy_wait_ms(200);  //Do work
	rt_printf("High task complete\n");

#ifdef use_mutex
	rt_mutex_release(&mutex);
#else
	rt_sem_v(&sem);
#endif
}

void sync_func(void *arg){
#ifdef use_mutex
	
#else
	rt_task_sleep(1000*1000*100);
	rt_printf("Running other tasks from the synchronization task\n");
	rt_sem_broadcast(&sem);
	rt_task_sleep(1000*1000*100);
#endif
}

void delete_all_sems(){
	rt_sem_delete(&sem);
	rt_mutex_delete(&mutex);
}

void busy_wait_ms(unsigned long delay)
{
	unsigned long count = 0;
	while (count <= delay*10){
		rt_timer_spin(1000*100);
		count++;
	}
}
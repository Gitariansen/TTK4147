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

// Semaphores
RT_SEM sem;

// Tasks
RT_TASK high_priority_task;
RT_TASK med_priority_task;
RT_TASK low_priority_task;

#define number_of_tasks 2

int set_cpu(int cpu_number);
void low_task();
void med_task();
void sync_task();
void delete_all_sems();

int main(){
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);

	//create task names
	char* high_name = "high";
	char* med_name = "med";
	char* low_name = "low";
	// create tasks
	rt_task_create(&high_priority_task, "high", 0, 3, (T_CPU(1) | T_JOINABLE));
	rt_task_create(&med_priority_task, "med", 0, 2, (T_CPU(1) | T_JOINABLE));
	rt_task_create(&low_priority_task, "low", 0, 1, (T_CPU(1) | T_JOINABLE));
	// create semaphore
	rt_sem_create(&sem, "high_sem", 0, S_FIFO);

	// start tasks
	rt_task_start(&low_priority_task, &low_task, NULL);
	rt_task_start(&med_priority_task, &med_task, NULL);
	rt_task_start(&high_priority_task, &sync_task, NULL);

	//join all tasks
	rt_task_join(&high_priority_task);
	rt_task_join(&med_priority_task);
	rt_task_join(&low_priority_task);

	delete_all_sems();
	return 0;
}


void low_task(){
	rt_sem_p(&sem, TM_INFINITE);

	rt_printf("Low task complete\n");

	rt_sem_v(&sem);
}

void med_task(){
	rt_sem_p(&sem, TM_INFINITE);

	rt_printf("Medium task complete\n");

	rt_sem_v(&sem);
}

void sync_task(void *arg){
	rt_task_sleep(1000*1000*100);
	rt_printf("Running other tasks from the synchronization task\n");
	rt_sem_broadcast(&sem);
	rt_task_sleep(1000*1000*100);
}

void delete_all_sems(){
	rt_sem_delete(&sem);
}

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}
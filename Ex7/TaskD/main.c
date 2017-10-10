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

// Mutexs
RT_MUTEX mutexA;
RT_MUTEX mutexB;

// Mutex priority
int mutexACeiling = 3;
int mutexBCeiling = 3;

// Tasks
RT_TASK sync_task;
RT_TASK high_priority_task;
RT_TASK low_priority_task;

#define number_of_tasks 2
#define use_mutex true

void low_task();
void high_task();
void delete_all_sems();
void busy_wait_ms(unsigned long delay);

int main(){
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);

	// create tasks
	rt_task_create(&high_priority_task, "high", 0, 3, (T_CPU(1) | T_JOINABLE));
	rt_task_create(&low_priority_task, "low", 0, 1, (T_CPU(1) | T_JOINABLE));
	// create semaphore
	rt_mutex_create(&mutexA, "elsa_sin_muti");
	rt_mutex_create(&mutexB, "lasse_sin_muti");

	// start tasks
	rt_printf("[INFO]: Start all tasks\n");
	rt_task_start(&low_priority_task, &low_task, NULL);
	rt_task_start(&high_priority_task, &high_task, NULL);

	//join all tasks
	rt_printf("[INFO]: Waiting for all tasks to join\n");
	rt_task_join(&high_priority_task);
	rt_task_join(&low_priority_task);

	delete_all_sems();
	return 0;
}


void low_task(){
	rt_printf("[TASK L]: Waits for mutexA...\n");
	rt_mutex_acquire(&mutexA, TM_INFINITE);	
	rt_task_set_priority(&low_priority_task, mutexACeiling);
	rt_task_sleep(1);
	rt_printf("[TASK L]: ... acquired mutexA\n");
	busy_wait_ms(300); //Do work
	rt_printf("[TASK L]: Waits for mutexB...\n");
	rt_mutex_acquire(&mutexB, TM_INFINITE);
	rt_task_set_priority(&low_priority_task, mutexBCeiling);
	rt_task_sleep(1);
	rt_printf("[TASK L]: ... acquired mutexB\n");
	busy_wait_ms(300); //Do workte
	rt_mutex_release(&mutexB);
	rt_printf("[TASK L]: Released mutexB\n");
	rt_mutex_release(&mutexA);
	rt_task_set_priority(&low_priority_task, 1);
	rt_task_sleep(1);
	rt_printf("[TASK L]: Released mutexA\n");
	busy_wait_ms(100); //Do work

	rt_printf("Low task complete\n");
}

void high_task(){
	rt_task_sleep(1000*1000*100);
	rt_printf("[TASK H]: Waits for mutexB...\n");
	rt_mutex_acquire(&mutexB, TM_INFINITE);
	rt_task_set_priority(&high_priority_task, mutexBCeiling);
	rt_task_sleep(1);
	rt_printf("[TASK H]: ... acquired mutexB\n");
	busy_wait_ms(100); //Do work
	rt_printf("[TASK H]: Waits for mutexA...\n");
	rt_mutex_acquire(&mutexA, TM_INFINITE);
	rt_task_set_priority(&high_priority_task, mutexACeiling);
	rt_task_sleep(1);
	rt_printf("[TASK H]: ... acquired mutexA\n");
	busy_wait_ms(200);  //Do work
	rt_mutex_release(&mutexA);
	rt_printf("[TASK H]: Released mutexA\n");
	rt_mutex_release(&mutexB);
	rt_task_set_priority(&high_priority_task, 3);
	rt_task_sleep(1);
	rt_printf("[TASK H]: Released mutexB\n");
	busy_wait_ms(100);  //Do work

	rt_printf("High task complete\n");
}

void delete_all_sems(){
	rt_mutex_delete(&mutexA);
	rt_mutex_delete(&mutexB);
}

void busy_wait_ms(unsigned long delay)
{
	unsigned long count = 0;
	while (count <= delay*10){
		rt_timer_spin(1000*100);
		count++;
	}
}
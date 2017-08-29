#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>


/* 


    real < user: The process is CPU bound and takes advantage of parallel execution on multiple cores/CPUs.
    real ≈ user: The process is CPU bound and takes no advantage of parallel exeuction.
    real > user: The process is I/O bound. Execution on multiple cores would be of little to no advantage.





    Real is wall clock time - time from start to finish of the call. This is all elapsed time including time slices used by other processes and time the process spends blocked (for example if it is waiting for I/O to complete).

    User is the amount of CPU time spent in user-mode code (outside the kernel) within the process. This is only actual CPU time used in executing the process. Other processes and time the process spends blocked do not count towards this figure.

    Sys is the amount of CPU time spent in the kernel within the process. This means executing CPU time spent in system calls within the kernel, as opposed to library code, which is still running in user-space. Like 'user', this is only CPU time used by the process. See below for a brief description of kernel mode (also known as 'supervisor' mode) and the system call mechanism.


*/


void busy_wait_delay(int seconds)
{
	int i, dummy;
	int tps = sysconf(_SC_CLK_TCK);
	clock_t start;
	struct tms exec_time;
	times(&exec_time);
	start = exec_time.tms_utime;
	while( (exec_time.tms_utime - start) < (seconds * tps))
		{
		for(i=0; i<1000; i++)
		{
			dummy = i;
		}
		times(&exec_time);
	}
}

void *thread1(){
	printf("Hello \n");
	//sleep(5);
	busy_wait_delay(5);
}

void *thread2(){
	printf("Hello \n");
	//sleep(5);
	busy_wait_delay(5);
}



int main(){
	pthread_t pthread1;
	pthread_t pthread2;
	//float startTime = gettimeofday();
	
	pthread_create(&pthread1, NULL, thread1, NULL);
	pthread_create(&pthread2, NULL, thread2, NULL);
	//float endTime = gettimeofday();
	
	pthread_join(pthread1, NULL);
	pthread_join(pthread2, NULL);
	
	return 0;

}

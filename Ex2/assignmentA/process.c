#include <stdio.h>
#include <unistd.h>

static int globalVariable = 0;


int main()
{
	int localVariable = 0;
	pid_t pid = fork();
	//fork();
	//pid = getpid();
	
	if (pid == 0)
	{ 
		//child process
		int i;
		for(i = 0; i < 5; i++){
			globalVariable++;
			localVariable++;
		}
		printf("%d From child process:\n Global = %d \n Local = %d \n", pid, globalVariable, localVariable);
	}
	else if (pid > 0)
	{
		//parent process
		int j;
		for(j = 0; j < 4; j++){
			globalVariable++;
			localVariable++;
		}
		printf("%d From parent process:\n Global = %d \n Local = %d \n", pid, globalVariable, localVariable);
		
	}
	
	else
	{
		//fork failed
		printf("fork() failed\n");
		return 1;
	}
	return 0;
	
	
}




//Any variable local to the parent process will have been copied for the child process. Updating a variable in one process will not affect the other

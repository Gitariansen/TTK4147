#include <stdio.h>
#include <stdlib.h>

void allocation(int value){
	int *ptr = NULL;
	ptr = malloc(1024*1024*sizeof(char));
	if(ptr == NULL)
	{
		perror("Error: ");
	}
	*ptr = value;
	printf("test of allocated memory: %i \n", *ptr);
}

int main()
{
	while(1)
	{
		allocation(42);
	}
}




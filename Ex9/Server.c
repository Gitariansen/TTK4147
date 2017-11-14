#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>

typedef struct pid_data{
	pthread_mutex_t pid_mutex;
	pid_t pid;
} PidData;

typedef struct Msg{
	char buffer[32];
	char data[32];
} msg;

int set_priority(int priority)
{
	int policy;
	struct sched_param param;

	// check priority in range
	if (priority < 1 || priority > 63) return -1;

	// set priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	param.sched_priority = priority;
	return pthread_setschedparam(pthread_self(), policy, &param);
}

int get_priority()
{
	int policy;
	struct sched_param param;

	// get priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	return param.sched_curpriority;
}

void *server(void);

int main(int argc, char *argv[]) {
	printf("Creating shared memory \n");

	pthread_t serverThread;

	pthread_create(&serverThread, NULL, server, NULL);

	pthread_join(serverThread, NULL);


	return EXIT_SUCCESS;
}



void *server(void){

	int channelID = ChannelCreate(0);
	int receivedMsgID;
	msg message;
	strcpy(message.data, "Ack from server");
	struct _msg_info* msgInfo;
	set_priority(7);
	int serverPriority;

	int fileDes = shm_open("/sharepid", O_RDWR|O_CREAT, S_IRWXU);					// Create pid file descriptor
	ftruncate(fileDes, sizeof(PidData));														// Resize memory
	PidData* pSharedMemLoc = mmap(0, sizeof(PidData), PROT_READ | PROT_WRITE, MAP_SHARED, fileDes, 0);

	pthread_mutexattr_t* attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&pSharedMemLoc->pid_mutex, &attr);

	pthread_mutex_lock(&pSharedMemLoc->pid_mutex);
	pSharedMemLoc->pid = getpid();
	pthread_mutex_unlock(&pSharedMemLoc->pid_mutex);

	printf("waiting for messages...\n");
	while(1){
		serverPriority = get_priority();
		printf("Server priority before received message %d \n", serverPriority);
		receivedMsgID = MsgReceive(channelID, &message.buffer, sizeof(message.buffer), msgInfo);
		serverPriority = get_priority();
		printf("Server priority after received message %d \n", serverPriority);

		printf(">>Received message: %s \n", &message.buffer);
		printf("msgInfo->pid: %d\n", &msgInfo->pid);
		printf("msgInfo->tid: %d\n", &msgInfo->tid);
		MsgReply(receivedMsgID, EOK, &message.data, sizeof(message.data));
	}
}


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
	int priority;
} msg;

void *sendMsg(void* Msg);

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

int main(int argc, char *argv[]) {
	printf("Reading PID\n");

	set_priority(50);
    msg message1;
    strcpy(message1.data, "hello from thread1");
    message1.priority = 1;

    msg message2;
    strcpy(message2.data, "hello from thread2");
    message2.priority = 2;

    msg message3;
	strcpy(message3.data, "hello from thread3");
	message3.priority = 13;

	msg message4;
	strcpy(message4.data, "hello from thread4");
	message4.priority = 14;

	//int fileDes = shm_open("/sharepid", O_RDWR, S_IRWXU);								// Resize memory
	//PidData* pSharedMemLoc = mmap(0, sizeof(PidData), PROT_READ | PROT_WRITE, MAP_SHARED, fileDes, 0);
	//pid_t sharedMemPID = pSharedMemLoc->pid;
	//printf("Shared mem pid: %d", sharedMemPID);

	//strcpy(message.data, "hello from client");

	//int channelID = ConnectAttach(0, sharedMemPID, 1, 0, 0);
	//int serverStatus = MsgSend(channelID, &message.data, sizeof(message.data), &message.buffer, sizeof(message.buffer));
	//ConnectDetach(channelID);

	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;

	pthread_create(&thread1, NULL, sendMsg, &message1);
	pthread_create(&thread2, NULL, sendMsg, &message2);
	pthread_create(&thread3, NULL, sendMsg, &message3);
	pthread_create(&thread4, NULL, sendMsg, &message4);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	pthread_join(thread4, NULL);

	return EXIT_SUCCESS;
}


void *sendMsg(void* Msg){
	msg message = *((msg*)Msg);

	set_priority(message.priority);

	struct _msg_info* msgInfo;
	int fileDes = shm_open("/sharepid", O_RDWR, S_IRWXU);								// Resize memory
	PidData* pSharedMemLoc = mmap(0, sizeof(PidData), PROT_READ | PROT_WRITE, MAP_SHARED, fileDes, 0);
	pid_t sharedMemPID = pSharedMemLoc->pid;
	//printf("Shared mem pid: %d\n", sharedMemPID);

	int channelID = ConnectAttach(0, sharedMemPID, 1, 0, 0);
	int serverStatus = MsgSend(channelID, &message.data, sizeof(message.data), &message.buffer, sizeof(message.buffer));

	int receivedMsgID = MsgReceive(channelID, &message.buffer, sizeof(message.buffer), msgInfo);
	printf(">>Received message from server: %s \n", &message.buffer);

	ConnectDetach(channelID);
}

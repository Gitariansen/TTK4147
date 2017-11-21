#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <semaphore.h>

dispatch_t              *dpp;
resmgr_attr_t           resmgr_attr;
dispatch_context_t      *ctp;
resmgr_connect_funcs_t  connect_funcs;
resmgr_io_funcs_t       io_funcs;
iofunc_attr_t           io_attr;

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);
void *counter_thread(void* arg);

//char buf[] = "Hello World\n";

char buffer[128];
pthread_mutex_t buffer_mutex;

int counter = 0;
pthread_mutex_t counter_mutex;

char counter_buffer[32];
pthread_mutex_t counter_buffer_mutex;
sem_t counter_msg_rec;




void error(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	printf("Start resource manager\n");

	// create dispatch.
	if (!(dpp = dispatch_create())){
		error("Create dispatch\n");
	}
	printf("DISPATCHED SUCCESSFULLY CREATED\n");

	// initialize a thread for setting the counter variable, and its variables
	pthread_t counter_update_thread;
	sem_init(&counter_msg_rec, 0, 0);
	pthread_create(&counter_update_thread, NULL, counter_thread, NULL);

	printf("INITIALIZED THEADS AND SEMAPHORES\n");

	// initialize resource manager attributes.
	memset(&resmgr_attr, 0, sizeof(resmgr_attr));
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = 2048;
	printf("INITIALIZED RESOURCE MANAGER ATTRIBUTES\n");

	// set standard connect and io functions.
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
	iofunc_attr_init(&io_attr, S_IFNAM | 0666, 0, 0);
	printf("SET STANDARD CONNECTION\n");

	// override functions
	io_funcs.read = io_read;
	io_funcs.write = io_write;
	printf("OVERRIDED IO RW FUNCTIONS\n");

	// establish resource manager
	if (resmgr_attach(dpp, &resmgr_attr, "/dev/myresource", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr) < 0)
		error("Resmanager attach");
	ctp = dispatch_context_alloc(dpp);
	printf("RESOURCE MANANGER ESTABLISHED\n");

	// wait forever, handling messages.
	while(1)
	{
		if (!(ctp = dispatch_block(ctp))){
			printf("ERROR 2\n");
			error("Dispatch block\n");
		}
		dispatch_handler(ctp);
	}
	pthread_join(counter_update_thread, NULL);
	exit(EXIT_SUCCESS);
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb)
{
	//pthread_mutex_t buffer_mutex;
	//pthread_mutex_init(&buffer_mutex);

	if(!ocb->offset)
	{
		pthread_mutex_lock(&buffer_mutex);

		pthread_mutex_lock(&counter_mutex);
		sprintf(buffer, "%d", counter);
		pthread_mutex_unlock(&counter_mutex);
		// set data to return
		SETIOV(ctp->iov, buffer, strlen(buffer));
		_IO_SET_READ_NBYTES(ctp, strlen(buffer));
		pthread_mutex_unlock(&buffer_mutex);

		// increase the offset (new reads will not get the same data)
		ocb->offset = 1;

		// return
		return (_RESMGR_NPARTS(1));
	}
	else
	{
		// set to return no data
		_IO_SET_READ_NBYTES(ctp, 0);

		// return
		return (_RESMGR_NPARTS(0));
	}
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb){

	/* set up the number of bytes (returned by client's write()) */

	_IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);


	pthread_mutex_lock(&buffer_mutex);
	if(buffer[0] == NULL){
		return(ENOMEM);
	}

	/*
	 *  Reread the data from the sender's message buffer.
	 *  We're not assuming that all of the data fit into the
	 *  resource manager library's receive buffer.
	 */

	resmgr_msgread(ctp, buffer, msg->i.nbytes, sizeof(msg->i));
	buffer [msg->i.nbytes] = '\0'; /* just in case the text is not NULL terminated */
	printf ("Received %d bytes = '%s'\n", msg -> i.nbytes, buffer);

	if(strncmp(buffer, "up", 2) == 0){
		pthread_mutex_lock(&counter_buffer_mutex);
		strncpy(counter_buffer, "up", 3);
		pthread_mutex_unlock(&counter_buffer_mutex);
		sem_post(&counter_msg_rec);
	}
	else if(strncmp(buffer, "down", 4) == 0){
		pthread_mutex_lock(&counter_buffer_mutex);
		strncpy(counter_buffer, "down", 5);
		pthread_mutex_unlock(&counter_buffer_mutex);
		sem_post(&counter_msg_rec);
	}
	else if(strncmp(buffer, "stop", 4) == 0){
		pthread_mutex_lock(&counter_buffer_mutex);
		strncpy(counter_buffer, "stop", 5);
		pthread_mutex_unlock(&counter_buffer_mutex);
		sem_post(&counter_msg_rec);
	}

	pthread_mutex_unlock(&buffer_mutex);



	return (_RESMGR_NPARTS(0));
}

void *counter_thread(void* arg){
	int stop_thread = 0;
	while(!stop_thread){
		sem_wait(&counter_msg_rec);
		pthread_mutex_lock(&counter_buffer_mutex);
		if(strncmp(counter_buffer, "up", 3) == 0){
			pthread_mutex_lock(&counter_mutex);
			counter += 1;
			pthread_mutex_unlock(&counter_mutex);
		}
		else if(strncmp(counter_buffer, "down", 5) == 0){
			pthread_mutex_lock(&counter_mutex);
			counter -= 1;
			pthread_mutex_unlock(&counter_mutex);
		}
		else if(strncmp(counter_buffer, "stop", 5) == 0){
			stop_thread = 1;
		}
		pthread_mutex_unlock(&counter_buffer_mutex);

		delay(100);
	}
}



#include <stdio.h>
#include "client/miniproject.h"

#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

#define SERVER_PORT 	9999
#define SERVER_IP 		"192.168.0.1"
#define PERIOD 			10000
#define REFERENCE		1

struct server_commands{
	char* START;
	char* GET;
	char* SET;
	char* STOP;
};

void *program_controller(void* commands);
void *pi_controller();
void *server_listener();
void *server_responder();
void server_commands_init(struct server_commands *commands);

float y;
struct server_commands server_commands;

/*
#####################################
######## MAAAAAAIN LOOOOOP ##########
#####################################
*/
int main(){
	printf("Starting tainyproject main..\n");
	
	// Initiate the server commands struct
	server_commands_init(&server_commands);
	
	// Initiate UDP-connection
	struct udp_conn UDP_connector;
	udp_init_client(&UDP_connector, SERVER_PORT, SERVER_IP);
	
	// Start simulation
	udp_send(&UDP_connector, server_commands.START, strlen(server_commands.START) + 1);
	
	pthread_t program_controller_thread;
	//pthread_t pi_controller_thread;
	//pthread_t server_listener_thread;
	//pthread_t server_responder_thread;

	pthread_create(&program_controller_thread, NULL, program_controller, &UDP_connector);
	//pthread_create(&pi_controller_thread, NULL, pi_controller, NULL);
	//pthread_create(&server_listener_thread, NULL, server_listener, NULL);
	//pthread_create(&server_responder_thread, NULL, server_listener, NULL);
	
	pthread_join(program_controller_thread, NULL);
	//pthread_join(pi_controller_thread, NULL);
	//pthread_join(server_listener_thread, NULL);
	//pthread_join(server_responder_thread, NULL);
	
	// Stop simulation & close udp connection
	udp_send(&UDP_connector, server_commands.STOP, strlen(server_commands.STOP) + 1);
	udp_close(&UDP_connector);
	
	return 0;
}





void *pi_controller(){
	int k_p = 10;
	int k_i = 800;
	float integral = 0;
	float error = 0;
	float u = 0;


	
	error = REFERENCE - y;
	integral = integral + (error * PERIOD);
	u = k_p * error + k_i * integral;
}

void *server_listener(){
	struct udp_conn *UDP_connector;

	udp_init_client(&UDP_connector, 9999, "192.168.0.1");
	
	while(1){
		udp_send(&UDP_connector, "random", 7);
		sleep(1);
	}
	
	udp_close(&UDP_connector);
}

void *program_controller(void* udp_connector){

	udp_conn udpConnector = *((udp_conn*)udp_connector);

	struct timespec next;
	struct timespec startTime;
	struct timespec endTime;
	clock_gettime(CLOCK_REALTIME, &next);
	clock_gettime(CLOCK_REALTIME, &startTime);
	
	while(endTime.tv_nsec - startTime.tv_nsec < 500000000){
		
		udp_send(&udpConnector, server_commands.GET, strlen(server_commands.GET) + 1);
	
		timespec_add_us(&next, PERIOD);
		clock_nanosleep(&next);
		clock_gettime(CLOCK_REALTIME, &endTime);
	}
	

}


void *server_responder(){
	int i;
	
}

void server_commands_init(struct server_commands *commands){
	commands->START = "START";
	commands->GET = "GET";
	commands->SET = "SET";
	commands->STOP = "STOP";
}

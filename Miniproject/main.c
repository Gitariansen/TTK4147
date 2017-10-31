#include <stdio.h>
#include <stdlib.h>
#include "client/miniproject.h"

#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <semaphore.h>

#define SERVER_PORT 	9999
#define SERVER_IP 		"192.168.0.1"
#define PERIOD_US 		2000
#define RUNTIME_US		500000

static const float reference = 1.0;

struct server_commands{
	char* START;
	char* GET;
	char* GET_ACK;
	char* SET;
	char* STOP;
};

void *program_controller(void* commands);
void *pi_controller(void* udp_connector);
void *server_listener(void* udp_connector);
void *server_responder();
void server_commands_init(struct server_commands *commands);

float y;
struct server_commands server_commands;

sem_t y_received;

pthread_mutex_t y_lock;

/*
#####################################
######## MAAAAAAIN LOOOOOP ##########
#####################################
*/

//TODO synchronise everything!


int main(){
	printf("Starting tainyproject main..\n");
	
	// Initiate the server commands struct
	server_commands_init(&server_commands);
	
	// Initiate UDP-connection
	struct udp_conn UDP_connector;
	udp_init_client(&UDP_connector, SERVER_PORT, SERVER_IP);
	
	// Init sem - move this somewhere nice
	sem_init(&y_received, 0, 0);

	
	// Start simulation
	udp_send(&UDP_connector, server_commands.START, strlen(server_commands.START) + 1);
	
	pthread_t program_controller_thread;
	pthread_t pi_controller_thread;
	pthread_t server_listener_thread;
	//pthread_t server_responder_thread;

	pthread_create(&program_controller_thread, NULL, program_controller, &UDP_connector);
	pthread_create(&pi_controller_thread, NULL, pi_controller, &UDP_connector);
	pthread_create(&server_listener_thread, NULL, server_listener, &UDP_connector);
	//pthread_create(&server_responder_thread, NULL, server_listener, NULL);
	
	pthread_join(program_controller_thread, NULL);

	
	// Stop simulation & close udp connection
	udp_send(&UDP_connector, server_commands.STOP, strlen(server_commands.STOP) + 1);
	udp_close(&UDP_connector);
	
	return 0;
}





void *pi_controller(void* udp_connector){

	udp_conn udpConnector = *((udp_conn*)udp_connector);
	
	int k_p = 10;
	int k_i = 800;
	float integral = 0;
	float error = 0;
	float u = 0;
	char u_str[32];
	float period_s = PERIOD_US/100000;
	
	printf("PI-controller started..\n");
	while(1){
	
		//sem_wait(&y_received);
		//pthread_mutex_lock(&y_lock);
		
		error = reference - y;
		integral = integral + (error * period_s);
		u = (k_p * error) + (k_i * integral);
		
		printf("Value of u:		%f \n", u);		
		sprintf(u_str, "SET:%f", u);
		
		//pthread_mutex_unlock(&y_lock);

	
		udp_send(&udpConnector, u_str, strlen(u_str) + 1);

	}
	
}


void *server_listener(void* udp_connector){
	char y_str[32];
	udp_conn udpConnector = *((udp_conn*)udp_connector);
	char received_msg[32];
	while(1){
	

		if(udp_receive(&udpConnector, received_msg, sizeof(received_msg))){
			if(strncmp(received_msg, server_commands.GET_ACK, strlen(server_commands.GET_ACK)) == 0){
				
				//pthread_mutex_lock(&y_lock);
				
				strncpy(y_str, (received_msg + 8), 8); //TODO generalise 8? split at ":"?
				printf("	Value of y:		%s\n", y_str);	
				y = atof(y_str);
				
				//pthread_mutex_unlock(&y_lock);
				//sem_post(&y_received);
			}
		}	
	}

}
void *program_controller(void* udp_connector){

	udp_conn udpConnector = *((udp_conn*)udp_connector);

	struct timespec next;
	int counter = 0;
	clock_gettime(CLOCK_REALTIME, &next);
	
	while(counter < (RUNTIME_US/PERIOD_US)){
		udp_send(&udpConnector, server_commands.GET, strlen(server_commands.GET) + 1);
		timespec_add_us(&next, PERIOD_US);
		clock_nanosleep(&next);
		counter++;
	}
}


void *server_responder(){
	int i;
	
}

void server_commands_init(struct server_commands *commands){
	commands->START 	= "START";
	commands->GET 		= "GET";
	commands->GET_ACK	= "GET_ACK";
	commands->SET 		= "SET";
	commands->STOP 		= "STOP";
}
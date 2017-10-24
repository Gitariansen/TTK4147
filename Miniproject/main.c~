#include <stdio.h>
#include "client/miniproject.h"

#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

void *UDP_listen();
void *PI_controller(double reference, double y);

int main(){
	printf("Starting tainyproject main..\n");
	
	//struct udp_conn *UDP_connector;
	//TODO pass argument into thread funciton
	
	
	pthread_t UDP_listen_thread;
	pthread_create(&UDP_listen_thread, NULL, UDP_listen, NULL);
	
	pthread_join(UDP_listen_thread, NULL);
	
	return 0;
}


/*
int Kp = 10;
int Ki = 800;
y - value read from simulation

error = reference - y;
integral = integral + (error * period);
u = Kp * error + Ki * integral

return u;
*/


void *UDP_listen(){
	struct udp_conn *UDP_connector;

	udp_init_client(&UDP_connector, 9999, "192.168.0.1");
	
	while(1){
		udp_send(&UDP_connector, "random", 7);
		sleep(1);
	}
	
	udp_close(&UDP_connector);
}

void *PI_controller(double reference, double y){
	int i;

}

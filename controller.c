#include "webcontent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8000
#define INVALID_SOCKET -1


/*gcc -pedantic -Wall -Wfatal-errors -std=c11  -O0 -o controller controller.c webcontent.c ../arduino-serial/arduino-serial-lib.o
*/


int main(int n, char** c){

	struct sockaddr_in addr;
	

	int a_socket, l_socket;
	int yes = 1;
	
	const char* device;

	if(n == 2){
		device = c[1];
		
	}else{
		fprintf(stdout,"Expected device path as argument for example /dev/ttyACM0 ...\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout,"Device is set to %s ...\n", device);
	

	addr.sin_port= htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;

	fprintf(stdout,"Starting the webinterface...\n");

	l_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(l_socket == INVALID_SOCKET){
		fprintf(stdout,"Failed to get a socket with error %d...\n", errno);
		return -1;
	}


	if(setsockopt(l_socket, SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1){
		fprintf(stdout,"Failed to setsockopt with error %d...\n", errno);
		close(l_socket);
		return -1;
	}

	if(bind(l_socket, (struct sockaddr*) &addr, sizeof(addr)) == -1){
		fprintf(stdout,"Failed to bind socket with error %d...\n", errno);
		return -1;
	}

	if(listen(l_socket,1)){
		fprintf(stdout,"Failed to listen to the socket with error %d...\n", errno);
		return -1;
	}
	/////////////////////////////////////////////////////////////

	fprintf(stdout,"Listening for incoming connections...\n");
	while(1){
		a_socket=accept(l_socket,NULL,NULL);
		if(a_socket == INVALID_SOCKET){
			fprintf(stdout,"Failed to accept a socket with error %d...\n", errno);
			break;
		}
		fprintf(stdout,"Got a connection...\n");

		if(handle_client_connection(a_socket,device) != 0){
			fprintf(stdout,"Failed to handle the client request...\n");
			break;
		}
		

	}

	fprintf(stdout,"Closing the main socket...\n");
	close(l_socket);

	return 0;
}

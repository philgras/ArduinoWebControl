#include "webcontent.h"
#include "arduino-serial-lib.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFERSIZE 8192



static int send_all(int c_socket, const char* buffer, int buf_len);

static int handle_get_request(int c_socket, const char* request, int len);
static int handle_post_request(int c_socket, const char* request, int len, int fd);


int handle_client_connection(int c_socket,const char* device){
	
	int rc = 0;
	int fd = -1;
	int bytes;
	char buffer[BUFFERSIZE];
	
	while(1){
		fprintf(stdout,"Receiving data...\n");
		bytes=recv(c_socket, buffer, BUFFERSIZE, 0);

		if(bytes==-1){

			fprintf(stdout,"Failed to receive from a socket with error %d...\n", errno);
			/**/
			rc = -1;
			break;

		}
		else if(bytes == 0 ){

			fprintf(stdout,"The connection was closed by the client...\n");
			close(c_socket);
			rc = 0;
			break;

		}else{
			
			if(0==strncmp("POST /index.html", buffer, strlen("POST /index.html"))){
				if(fd == -1){
					fd = serialport_init(device,9600);
					if(fd == -1){
						fprintf(stdout,"Failed to connect to Arduino...\n");
						rc = -1;
						break; 
					}
				}

				fprintf(stdout,"Processing post request...\n");
				rc = handle_post_request(c_socket,buffer, bytes, fd);

				if(rc == -1){
					fprintf(stdout,"Failed to handle the post_request...\n");					
					break;
				}else if(rc == 0){
					fprintf(stdout,"Client refuses keep-alive...\n");					
					break;
				}
				
				continue;

			}else if(0==strncmp("GET /index.html", buffer, strlen("GET /index.html"))){

				fprintf(stdout,"Processing get request...\n");
				rc = handle_get_request(c_socket,buffer, bytes);
				
				break;

			}else{

				buffer[bytes] = '\n';
				fprintf(stdout,"Wrong request.\nRequest: %s\n", buffer);
				/***/
				rc = -1;
				break;

			}
			
		}

	}

	if(fd != -1){
		fprintf(stdout,"Closing connection to arduino...\n");
		serialport_close(fd);
	}

	fprintf(stdout,"Closing the connection...\n");
	close(c_socket);
	return rc;

}


static int handle_get_request(int c_socket, const char* request, int len){
	
	static const char* response = "HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html; charset=utf-8;\r\n"
						"\r\n\r\n"
						"<html><head><title>Controller</title>"
						"<script>"
							"function sendReq(){" //change this to <form method="post"> when not needed anymore
							"var ajax = new XMLHttpRequest();"
							"ajax.open('POST','/index.html?red='+document.getElementById('red').value+"
															"'&green='+document.getElementById('green').value+ "
															"'&blue='+document.getElementById('blue').value,true);"
							"ajax.addEventListener('load', function(){console.log(\"done\");});"
							"ajax.send();"
							"}"
						"</script>"
						"</head><body>"
						"<div>"
							"Red <br><input style=\"width: 100%;\" id=\"red\" type=\"number\" min=\"0\" max=\"255\"/><br>"
							"Green<br><input style=\"width: 100%;\" id=\"green\" type=\"number\" min=\"0\" max=\"255\"/><br>"
							"Blue<br><input style=\"width: 100%;\" id=\"blue\" type=\"number\" min=\"0\" max=\"255\"/><br>"
						"<div>"
						
							"<button type=\"button\" onClick=\"sendReq()\">Send</button>"
						
						"</body><html>";

	

	return send_all(c_socket,response, strlen(response));

}

static int handle_post_request(int c_socket,const char* request, int len, int fd){

	//request validieren + parameter auslesen
	//connection: keep alive prüfen
	//response senden

	static char buffer[32];
	int size = 0;
	static const char* conn_field="Connection: keep-alive";
	static const char* red = "red=";
	static const char* green = "green=";
	static const char* blue = "blue=";
	static const char* post_response_keep_alive = "HTTP/1.1 200 OK\r\n"
												"Content-Type: text/json; charset=utf-8;\r\n"
												"Content-Length: 2\r\n"
												"Connection: keep-alive\r\n"
												"\r\n\r\n{}";
	static const char* post_response_close = "HTTP/1.1 200 OK\r\n"
												"Content-Type: text/json; charset=utf-8;\r\n"
												"Content-Length: 2\r\n"
												"Connection: close\r\n"
												"\r\n\r\n{}";
	
	int rc;

	const char* begin = strstr(request+5,red) + strlen(red);
	const char* end = strchr(begin,'&');
	assert(begin!=NULL);
	assert(end-begin < 9);
	
	memcpy(buffer,begin,end-begin);
	buffer[end-begin] = ';';
	size += (end-begin) +1;
	

	begin = strstr(request+5,green) + strlen(green);
	end = strchr(begin,'&');
	assert(begin!=NULL);
	assert(end-begin < 9);
	
	memcpy(buffer+size,begin,end-begin);
	buffer[(end-begin)+size] = ';';
	size += (end-begin) +1;

	begin = strstr(request+5,blue) + strlen(blue);
	end = strchr(begin,' ');
	assert(begin!=NULL);
	assert(end-begin < 9);
	
	memcpy(buffer+size,begin,end-begin);
	buffer[(end-begin)+size] = ';';
	buffer[(end-begin)+size+1] = '\0';
	serialport_write(fd,buffer);


	begin = strstr(request,conn_field);
	if(begin != NULL){
		rc = send_all(c_socket,post_response_keep_alive, strlen(post_response_keep_alive)) 
					== -1 ? -1 : 1;
		
	}else{
		rc = send_all(c_socket,post_response_close, strlen(post_response_close));
					
	}
	
	return rc;
	

}

static int send_all(int c_socket,const char* buffer, int buf_len){
	
	int bytes_send;
	int bytes=0;
	while( 1){
		bytes_send = send(c_socket, buffer+ bytes, buf_len,0);
		if(bytes_send == -1){
			fprintf(stdout,"Failed to send %d...\n", errno);
			
			return -1;
		}else if(buf_len>(bytes+=bytes_send)){
			fprintf(stdout,"Couldn't send all bytes at once...\n");
			buf_len = buf_len -bytes;
			continue;
		}
		break;
	}
	return 0; 
}



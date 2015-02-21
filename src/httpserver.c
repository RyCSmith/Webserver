/*
 * server_helper.h
 *
 *  Created on: Feb 15, 2014
 *      Author: ryancsmith 
 */
/*
Fundamental server operations were borrowed from the following 2 sources:
1 - http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
2 - http://www.binarii.com/files/papers/c_sockets.txt
This generally immediately follows numbered comments.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "server_helper.h"
#include <pthread.h>

int quit_signal = 0;
int success_request = 0;
int fail_request = 0;
int bytes_sent = 0;

/*
Configures server and loops, processing connections 1 by 1 until quit_signal is received
*/
void* server_thread(void* p){
      //get info regarding server config details
      server_info* info = (server_info*)p;
      int PORT_NUMBER = info->port_num;
      char* directory = info->directory;

      // structs to represent the server and client
      struct sockaddr_in server_addr,client_addr;
      int sock; // socket descriptor

      // 1. socket: creates a socket descriptor that you later use to make other system calls
      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("Socket");
      exit(1);
      }
      int temp;
      if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
      perror("Setsockopt");
      exit(1);
      }

      // configure the server
      server_addr.sin_port = htons(PORT_NUMBER); // specify port number
      server_addr.sin_family = AF_INET;
      server_addr.sin_addr.s_addr = INADDR_ANY;
      bzero(&(server_addr.sin_zero),8);

      // 2. bind: use the socket and associate it with the port number
      if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
      perror("Unable to bind");
      exit(1);
      }

      // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
      if (listen(sock, 1) == -1) {
      perror("Listen");
      exit(1);
      }

      // once you get here, the server is set up and about to start listening
      printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
      fflush(stdout);

      //loops, waiting for requests until quit_signal is activated
      while (quit_signal == 0){
            // 4. accept: wait here until we get a connection on that port
            int sin_size = sizeof(struct sockaddr_in);
            int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
            printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

            // buffer to read data into
            char request[1024];
            // 5. recv: read incoming request message into buffer
            int bytes_received = recv(fd,request,1024,0);
            request[bytes_received] = '\0';

            char* requested_item = requested_file(request);
            char* full_string;
            //if favicon - ignore, if stats - report, if file - process
            if (strcmp(requested_item, "/favicon.ico") == 0){
                  char* empty_string = "favicon";
                  send(fd, empty_string, strlen(empty_string), 0);
                  printf("favicon request acknowledged\n" );
                  free(requested_item);
                  close(fd);
                  continue;
            }
            else if (strcmp(requested_item, "/stats") == 0){
                  full_string = report_stats(success_request, fail_request, bytes_sent);
            }
            else{
                  char* path = extract_path(request, directory);
                  full_string = get_file(path, request);
                  free(path);
            }
            // 6. send: send the message over the socket
            // note that the second argument is a char*, and the third is the number of chars
            int completion_value = send(fd, full_string, strlen(full_string), 0);
            if (completion_value < 0){
                  fail_request += 1;
                  printf("Server failed to send message.");
            }
            else{
                  success_request += 1;
                  bytes_sent += completion_value;
                  printf("Server sent message: %s\n", full_string);
            }
            free(requested_item);
            free(full_string);
            close(fd);
      }
      // 7. close: close the socket connection
      close(sock);
      printf("Server closed connection\n");

      return 0;
}

/*
Waits for input from user. If input is Q or q, updates quit_signal.
*/
void* input_thread (void* p){
      //reads input into char array to avoid errors if extra keys are hit accidentally
      char user_input[100];
      while (user_input[0] != 'q' && user_input[0] != 'Q'){
            scanf("%s", user_input);      
      }
      quit_signal = 1;
      return NULL;
}

/*
Processes args, starts threads and eexecute program
*/
int main(int argc, char *argv[])
{
	// check the number of arguments
	if (argc != 3){
		printf("\nPlease enter the proper number of arguments when executing.\n");
		exit(0);
	}
      //package the arguments into a server_info struct and pass to server thread
	char* directory = argv[2];
      server_info* start_info = malloc(sizeof(server_info));
      if (start_info == NULL)
            printf("\nAn error occurred while allocating memory for your request. Please try again.\n");
      start_info->port_num = atoi(argv[1]);

      start_info->directory = malloc(sizeof(char) * strlen(argv[2]) + 1);
      if (start_info->directory == NULL)
            printf("\nAn error occurred while allocating memory for your request. Please try again.\n");
      strcpy(start_info->directory, argv[2]);

      //create and join threads
      pthread_t thread1, thread2;
      pthread_create(&thread1, NULL, &server_thread, (void*)start_info);
      pthread_create(&thread2, NULL, &input_thread, NULL);

      pthread_join(thread1, NULL);
      pthread_join(thread2, NULL);

      //free the server_info package once server has terminated execution
      free(start_info->directory);
      free(start_info);

}


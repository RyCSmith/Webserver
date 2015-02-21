/*
 * server_helper.c
 *
 *  Created on: Feb 15, 2014
 *      Author: ryancsmith
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

/*
Reads data from file and contructs string with appropriate header to be sent be to requester
Returns a 404 response if file not found
Returns a 500 response if memory could not be allocated or if error opening file that exists
*/
char* get_file(char* path, char* request){
      //calls get file size to determine how much space to calloc
      int file_size = get_file_size(path);
      char* full_string;
      //if file_size < 0 - file not found - return 404
      if (file_size > 0){
         full_string = calloc(1, file_size + 200);
         //if space could not be allocated return 500
         if (full_string == NULL)
            return process_500();
      }
      else
         return process_404(request);
      //the header that is always inserted
      char* header = "HTTP/1.1 200\n OK Content-Type: text/html\n\n";
      //gets the FILE pointer - error if NULL
      FILE *src_file = fopen (path, "r");
      if (src_file == NULL){
            //if file==NULL, process 404 
            free(full_string);       
            return process_500();
      }
      //copies header into full_string - remember to use strcpy (not cat) on the first word, cat will not know where to put it
      strcpy(full_string, header);
      //makes an array for a single word - gave this a full .1 mb to avoid errors with large html components
      char word[100000];
      char c;
      //loops while != EOF + appends each piece to full word
      c = fscanf(src_file, "%s", word);
      while (c != EOF) {
        strcat(full_string, word);
        strcat(full_string, " ");
              c = fscanf(src_file, "%s", word);
      }
      strcat(full_string, "\0");
      fclose(src_file);
      return full_string;
}
/*
Creates a 404 response string
*/
char* process_404(char* request){
      char* full_string = calloc(1, 2000);
      if (full_string == NULL)
         return "error";
      char* header404 = "HTTP/1.1 404 Not Found\n\n";
      strcpy(full_string, header404);
      char* file = requested_file(request);
      strcat(full_string, "Sorry, we were unable to locate ");
      strcat(full_string, file);
      strcat(full_string, "\0");
      free(file);
      return full_string;
}
/*
Creates a 500 response string
*/
char* process_500(){
      char* full_string = calloc(1, 200);
      if (full_string == NULL)
         return "error";
      char* header500 = "HTTP/1.1 500 Internal Server Error\n\n";
      strcpy(full_string, header500);
      strcat(full_string, "Sorry, an Internal Server Error Occurred.");
      strcat(full_string, "\0");
      return full_string;
}
/*
Returns a string containing the full directory path to requested file
Constructs path to index.html if no file specified
*/
char* extract_path(char* request, char* directory){
      char* file = requested_file(request);
      //creates the full path to be rendered
      char* path = calloc(1, 150);
      if (path == NULL)
         return "error";
      char* defaultPath = "/index.html";
      strcpy(path, directory);
      if (strlen(file) == 1)
        strcat(path, defaultPath);
      else
        strcat(path, file);
      free(file);
      return path;
}
/*
Returns a string containing the requested file
*/
char* requested_file(char* request){
      char s[2] = " ";
      char *token;
      //making a copy preserves the received request for later use if needed
      char* request_copy = calloc(1, sizeof(char) * strlen(request) + 1);
      //if memory could not be allocated return "error"
      if (request_copy == NULL)
         return "error";
      strcpy(request_copy, request);
      token = strtok(request_copy, s);
      char* filename = calloc(1, 50);
      //if memory could not be allocated return "error"
      if (filename == NULL)
         return "error";
      //we know that the filename is the second token
      token = strtok(NULL, s);
      strcpy(filename, token);
      free(request_copy);
      return filename;
}
/*
Constructs a string from current server stats for reporting
*/
char* report_stats(int success, int fail, int bytes){
      char string[1000];
      char* report_string = (char*)malloc(sprintf(string, "Total successfully processed requests: %d\nTotal failed requests: %d\nTotal bytes sent: %d\n", success, fail, bytes) + 1);
      //if memory could not be allocated, pass back error message for sending.
      if (report_string == NULL)
         return "Sorry, stats could not be processed at this time.";
      strcpy(report_string, string);
      return report_string;
}
/*
Returns the total size of the file to be transmitted
*/
int get_file_size (char* path){
   int length;
   FILE* src_file = fopen(path, "r");
   if( src_file == NULL ) 
      return(-1);
   fseek(src_file, 0, SEEK_END);
   length = ftell(src_file);
   fclose(src_file);   
   return(length);
}
     
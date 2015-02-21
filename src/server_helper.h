/*
 * server_helper.h
 *
 *  Created on: Feb 15, 2014
 *      Author: ryancsmith
 */

typedef struct ServerInfo server_info;
struct ServerInfo {
  int port_num;
  char* directory;
};

char* get_file(char*, char*);
char* process_404(char*);
char* process_500();
char* extract_path(char*, char*);
char* requested_file(char*);
char* report_stats(int, int, int);
int get_file_size(char*);


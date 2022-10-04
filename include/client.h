#ifndef ASSIGNMENT1_CLIENT_H
#define ASSIGNMENT1_CLIENT_H

#include "common.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>

#define DEFAULT_PORT 5000



struct options
{
    char *file_name;
    char *ip_out;
    in_port_t port_out;
    int fd_in;
    int fd_out;
    int server_socket;
    char* file_arr[50];
    int file_count;
};

typedef struct _finddata_t  FILE_SEARCH;


static void options_init(struct options *opts);
static void parse_arguments(int argc, char *argv[], struct options *opts);
static void options_process(struct options *opts);
static void cleanup(const struct options *opts);
void get_file_list(struct options *opts);


#endif //ASSIGNMENT1_CLIENT_H

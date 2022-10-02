#ifndef ASSIGN1_SERVER_H
#define ASSIGN1_SERVER_H

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <ctype.h>

#define DEFAULT_PORT 5000
#define DEFAULT_DIRECTORY "./server"

struct options_server
{
    char* file_name;
    char* ip_in;
    in_port_t port_in;
    char directory[50];
    int fd_in;
    int server_socket;
    int client_socket;
    char* file_arr[50];
    int file_count;
};


static void options_init_server(struct options_server *opts);
static void parse_arguments_server(int argc, char *argv[], struct options_server *opts);
static void options_process_server(struct options_server *opts);
static void cleanup_server(const struct options_server *opts);

#endif //ASSIGN1_SERVER_H

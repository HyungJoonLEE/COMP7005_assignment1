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

#define DEFAULT_DIRECTORY "."
#define BACKLOG 5


struct options_server
{
    in_port_t port_in;
    char directory[100];
    char* origin_directory;
    int server_socket;
    int client_socket[5];
    char file_name[20];
    int active_sd;
};


static void options_init_server(struct options_server *opts);
static void parse_arguments_server(int argc, char *argv[], struct options_server *opts);
static void options_process_server(struct options_server *opts);
static void cleanup_server(const struct options_server *opts);

#endif //ASSIGN1_SERVER_H

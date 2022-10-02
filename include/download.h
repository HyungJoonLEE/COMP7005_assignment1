#ifndef ASSIGNMENT1_DOWNLOAD_H
#define ASSIGNMENT1_DOWNLOAD_H

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
#include <sys/stat.h>
#include <sys/types.h>
#include "server.h"

#define BACKLOG 5

int mkdirs(const char *path, mode_t mode);
void download_file(struct options_server *opts);
void save_file(struct options_server *opts);
void remove_file(char* directory);

#endif //ASSIGNMENT1_DOWNLOAD_H

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
#include "server.h"

#define MAX_BUFFER	1024
#define BACKLOG 5
#define MAXBUF 1300

void download_file(struct options *opts);

#endif //ASSIGNMENT1_DOWNLOAD_H

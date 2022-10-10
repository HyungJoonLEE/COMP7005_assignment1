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


/**
 * Get parameter 'path', parse with '/', and use mkdir() for creating directories.
 *
 * @param path Serverside input for setting directory for download files from client
 * @param mode Give permission to that directory 0775(rwx-r-xr--)
 * @return
 */
int mkdirs(const char *path, mode_t mode);


/**
 * Download files process.
 * 1) Get file count from a client and send confirm message.
 * 2) Get file name from a client and send confirm message.
 * 3) Get file size from a client and send confirm message.
 * 4) Get file content from a client and send confirm message.
 *
 * @param opts server option struct settings
 */
void download_file(struct options_server *opts);


#endif //ASSIGNMENT1_DOWNLOAD_H

#ifndef ASSIGNMENT1_SEND_H
#define ASSIGNMENT1_SEND_H

#include <stddef.h>
#include "client.h"


/**
 * Send file(s) to server.
 * 1) Send file count to server and receive a confirm message.
 * 2) Send file name to server and receive a confirm message.
 * 3) Send file size to server and receive a confirm message.
 * 4) Send file content to server and receive a confirm message (Open file and read file 256 bytes).
 *
 * @param opts client option struct settings
 */
void send_file(struct options *opts);
#endif //ASSIGNMENT1_SEND_H

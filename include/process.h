#ifndef ASSIGNMENT1_PROCESS_H
#define ASSIGNMENT1_PROCESS_H


#include <stddef.h>
#include "client.h"

void option_process_server(struct options *opts);
void client_read_from_file(struct options *opts, size_t count);
#endif //ASSIGNMENT1_PROCESS_H

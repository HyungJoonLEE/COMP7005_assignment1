#ifndef ASSIGNMENT1_SEND_H
#define ASSIGNMENT1_SEND_H


#include <stddef.h>
#include "client.h"

void send_number_of_file(struct options *opts);
void send_file(struct options *opts, size_t count);
int get_confirm_message(struct options *opts);
#endif //ASSIGNMENT1_SEND_H

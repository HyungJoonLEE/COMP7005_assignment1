#ifndef ASSIGNMENT1_SEND_H
#define ASSIGNMENT1_SEND_H

#include <stdio.h>
#include "client.h"

void send_file(struct options *opts);
int files_parsing(char* user_input, char** text_file_arr);

#endif //ASSIGNMENT1_SEND_H

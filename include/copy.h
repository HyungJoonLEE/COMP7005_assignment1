#ifndef ASSIGNMENT1_COPY_H
#define ASSIGNMENT1_COPY_H


#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

void copy(int from_fd, int to_fd, size_t count);


#endif //ASSIGNMENT1_COPY_H

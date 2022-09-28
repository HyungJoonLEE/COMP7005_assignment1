#ifndef ASSIGNMENT1_CONVERSION_H
#define ASSIGNMENT1_CONVERSION_H

#include <netinet/in.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>


in_port_t parse_port(const char *buff, int radix);


#endif //ASSIGNMENT1_CONVERSION_H

//
// Created by Hyung Joon Lee on 2022-09-27.
//
#ifndef ASSIGN1_SERVER_H
#define ASSIGN1_SERVER_H

#define DEFAULT_PORT 8000
#define DEFAULT_DIRECTORY " ~/server"
#define INVALID 0


unsigned int ip_to_int (const char * ip);
void error_handling(char *message);

#endif //ASSIGN1_SERVER_H

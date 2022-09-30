#include "send.h"
#include "client.h"
#include "common.h"
#include "error.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void send_file(struct options *opts) {
    char* text_file_arr[15];
    int count = files_parsing(opts->send_file_name, text_file_arr);
    for (int i = 0; i < count; i++) {
        printf("send file %d: %s\n", i, text_file_arr[i]);
    }

    if(opts->file_name)
    {
        opts->fd_in = open(opts->file_name, O_RDONLY);

        if(opts->fd_in == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }
    }


    if(opts->ip_out)
    {
        int result;
        struct sockaddr_in server_address;

        opts->fd_out = socket(AF_INET, SOCK_STREAM, 0);

        if(opts->fd_out == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(opts->port_out);
        server_address.sin_addr.s_addr = inet_addr(opts->ip_out);

        if(server_address.sin_addr.s_addr ==  (in_addr_t)-1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        result = connect(opts->fd_out, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));

        if(result == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }
    }

    int server_soecket;
    size_t fsize, nsize = 0;
    size_t fsize2;
    FILE *file = NULL;
    char buf[1200];

    /* 전송할 파일 이름을 작성합니다 */
    file = fopen(text_file_arr[0], "rb");

    /* 파일 크기 계산 */
    // move file pointer to end
    fseek(file, 0, SEEK_END);
    // calculate file size
    fsize = ftell(file);
    // move file pointer to first
    fseek(file, 0, SEEK_SET);

    // send file size first
    // fsize2 = htonl(fsize);
    // send file size
    // send(serv_sock, &fsize2, sizeof(fsize), 0);

    // send file contents
    while (nsize!=fsize) {
        // read from file to buf
        // 1byte * 256 count = 256byte => buf[256];
        int fpsize = fread(buf, 1, 256, file);
        nsize += fpsize;
        send(server_soecket, buf, fpsize, 0);
    }

    fclose(file);
//    close(server_soecket);
}


int files_parsing(char* user_file, char** text_file_arr) {
    int num = 1;
    printf("%s", user_file);

    /**
    if (strcmp(user_file, "*.txt") == 0) {
        // do check files in system
    }
    */


    int count = 0;
    char* ptr = strtok(user_file, " ");
    count++;
    while (ptr != NULL) {
        ptr = strtok(NULL, " ");
        count++;
    }


    char* text_file = strtok(user_file, " ");
    text_file_arr[0] = text_file;
    if (count != 1) {
        for (int i = 1; i < count; i++) {
            text_file = strtok(NULL, " ");
            text_file_arr[i] = text_file;
        }
    }
    for (int i = 0; i < count; i++) {
        printf("%d : %s\n", i, text_file_arr[i]);
    }
    return count;
}

#include "server.h"
#include "download.h"
#include "error.h"
#include "send.h"



void download_file(struct options_server *opts) {

    ssize_t nbyte = 256;
    char received_content[BUFSIZ];
    size_t filesize = 0, bufsize = 0;
    FILE *file = NULL;


    file = fopen("total", "wb");
    bufsize = 256;

    while (/*filesize != 0*/nbyte != 0) {
        //if(filesize < 256) bufsize = filesize;
        nbyte = recv(opts->client_socket, received_content, bufsize, 0);
        fwrite(received_content, sizeof(char), (unsigned long) nbyte, file);
        //nbyte = 0;
    }
}


int mkdirs(const char *path, mode_t mode) {
    char tmp_path[100];
    const char *tmp = path;
    long len  = 0;
    int ret;

    if(path == NULL || strlen(path) >= 2048) {
        return -1;
    }

    while((tmp = strchr(tmp, '/')) != NULL) {
        len = tmp - path;
        tmp++;

        if(len == 0) {
            continue;
        }
        strncpy(tmp_path, path, len);
        tmp_path[len] = 0x00;

        if((ret = mkdir(tmp_path, mode)) == -1) {
            if(errno != EEXIST) {
                printf("Path exist\n");
                return -1;
            }
        }
    }
    return mkdir(path, mode);
}


void create_name_file(struct options_server *opts) {
    ssize_t nbyte = 256;
    char received_content[BUFSIZ];
    size_t bufsize = 0;
    FILE *file = NULL;


    file = fopen("name.txt", "wb");
    bufsize = 256;

    nbyte = recv(opts->client_socket, received_content, bufsize, 0);
    fwrite(received_content, sizeof(char), (unsigned long) nbyte, file);
}


void store_text_name(struct options_server *opts) {
    FILE* fp = fopen("total.txt", "r");
    char buffer[100] = { 0, };
    fread(buffer, 1, sizeof(buffer), fp);
    printf("%s", buffer);


//    char *ptr = strtok(buffer, " ");
//    while (ptr != NULL)
//    {
//        printf("%s\n", ptr);
//        ptr = strtok(NULL, " ");
//    }
    fclose(fp);
}




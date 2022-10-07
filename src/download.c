#include "server.h"
#include "download.h"
#include "error.h"
#include "send.h"


void download_file(struct options_server *opts) {

    ssize_t nbyte0;
    ssize_t nbyte;
    ssize_t nbyte2;
    ssize_t nbyte3;
    char received_file_count[BUF_SIZE];
    char received_content[BUF_SIZE];
    char received_content2[BUF_SIZE];
    char received_content3[BUF_SIZE];
    FILE *file = NULL;
    char buffer[256];
    ssize_t file_size = 0;
    ssize_t downloaded_size = 0;
    int file_count = 0;

    char confirm[8] = "CONFIRM";
    confirm[7] = '\0';
    char complete[9];
    complete[8] = '\0';
    char *ptr;
    int count = 0;

    while (TRUE) {
        read(opts->active_sd, received_file_count, sizeof(received_file_count));
        file_count = (int)strtol(received_file_count, NULL, 10);
        write(opts->active_sd, confirm, 8);
        break;
    }


    while (count < file_count) {
        printf("=== File %d ===\n", count);
        while (TRUE) {
            nbyte = read(opts->active_sd, received_content, sizeof(received_content));
//            printf("1 = %s\n", received_content);
            ptr = strtok(received_content, "\n");
            if (strstr(ptr, ".txt") != NULL) {
                strcpy(opts->file_name, received_content);
                printf("FILE NAME = %s\n", opts->file_name);
            }
            write(opts->active_sd, confirm, 8);
            break;
        }

        while (TRUE) {
            nbyte2 = read(opts->active_sd, received_content2, sizeof(received_content2));
            file_size = strtol(received_content2, NULL, 10);
            printf("FILE SIZE = %d\n", (int) file_size);
            write(opts->active_sd, confirm, 8);
            break;
        }

        file = fopen(opts->file_name, "wb");
        while (TRUE) {
            nbyte3 = read(opts->active_sd, received_content3, sizeof(received_content3));
//            printf("READ - %ld bytes\n", nbyte3);
//            printf("%s", received_content3);
            fwrite(received_content3, sizeof(char), (unsigned long) nbyte3, file);
            downloaded_size += nbyte3;
            if (downloaded_size == file_size) {
                write(opts->active_sd, confirm, 8);
                break;
            }
        }
        fclose(file);
        downloaded_size = 0;
        file_size = 0;
        count++;
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

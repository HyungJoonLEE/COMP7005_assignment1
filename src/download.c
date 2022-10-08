#include "server.h"
#include "download.h"
#include "error.h"
#include "send.h"


void download_file(struct options_server *opts) {

    ssize_t received_bytes;
    char received_file_count[BUF_SIZE];
    char received_file_name[14];
    char received_file_size[BUF_SIZE];
    char received_file_text[BUF_SIZE];
    FILE *file = NULL;
    char buffer[256];
    ssize_t file_size = 0;
    ssize_t downloaded_size = 0;
    int file_count = 0;

    char confirm[8] = "CONFIRM";
    confirm[7] = '\0';
    char complete[9];
    complete[8] = '\0';
    int count = 0;

    while (TRUE) {
        read(opts->active_sd, received_file_count, sizeof(received_file_count));
        file_count = (int)strtol(received_file_count, NULL, 10);
        printf("file count = %d\n", file_count);
        write(opts->active_sd, confirm, 8);
        break;
    }

    while (count < file_count) {
        printf("=== File %d ===\n", count + 1);
        while (TRUE) {
            read(opts->active_sd, received_file_name, sizeof(received_file_name));
//            printf("1 = %s\n", received_file_name);
            if (strstr(received_file_name, ".txt") != NULL) {
                strncpy(opts->file_name, received_file_name, strlen(received_file_name) - 1);
                strcat(opts->file_name, "\0");
                printf("FILE NAME = %s\n", opts->file_name);
            }
            write(opts->active_sd, confirm, 8);
            break;
        }


        while (TRUE) {
            read(opts->active_sd, received_file_size, sizeof(received_file_size));
            strcat(received_file_size, "\0");
            printf("2 = %s\n", received_file_size);
            file_size = strtol(received_file_size, NULL, 10);
            printf("FILE SIZE = %d\n", (int) file_size);
            write(opts->active_sd, confirm, 8);
            break;
        }

        file = fopen(opts->file_name, "wb");
        printf("file name %s\n",opts->file_name);
        while (received_bytes > 0) {
            received_bytes = read(opts->active_sd, received_file_text, sizeof(received_file_text));
//            printf("READ - %ld bytes\n", received_bytes);
//            printf("%s", received_file_text);
            fwrite(received_file_text, sizeof(char), (unsigned long) received_bytes, file);
            downloaded_size += received_bytes;
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

#include "server.h"
#include "download.h"
#include "error.h"
#include "send.h"
#include "common.h"


void send_confirm_message(struct options_server *opts) {
    char message[7] = "confirm";
    message[7] = '\0';
    write(opts->active_sd, message, strlen(message));
}


void get_number_of_file(struct options_server *opts) {
    char message[4];
    char *ptr = NULL;
    while(TRUE) {
        read(opts->active_sd, message, strlen(message));
        opts->file_count = (int)strtol(message, &ptr, 10);
        printf("server file count = %d\n", opts->file_count);
        return;
    }
}

void download_file(struct options_server *opts) {

    ssize_t nbyte = 256;
    char received_content[BUF_SIZE];
    char message[100];
    size_t filesize = 0, bufsize = 0;
    unsigned long current_size = 0;
    FILE *file = NULL;


    bufsize = 256;
    int i = 0;

    while (nbyte != 0) {
        ntohl(filesize);
        recv(opts->active_sd, &filesize, sizeof(filesize), 0);
        printf("file size = %ld | nbyte = %d\n", filesize, nbyte);
        nbyte = recv(opts->active_sd, received_content, bufsize, 0);

//        if (strstr(received_content, ".txt") != NULL) {
//            printf("text name = %s\n", received_content);
//            opts->file_arr[i] = malloc(sizeof(char) * strlen(received_content));
//            strcpy(opts->file_arr[i], received_content);
//            printf("this is file = %s\n", opts->file_arr[i]);
//            file = fopen(opts->file_arr[i], "wb");
//            i++;
//        }
//        fwrite(received_content, sizeof(char), (unsigned long) nbyte, file);
//        printf("%s\n", received_content);
//        printf("=======256=======\n");
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


void save_file(struct options_server *opts) {
    const char* filename = "total.txt";
    char *storage;

    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    char* file_contents = malloc((unsigned long) sb.st_size);
    fread(file_contents, (unsigned long) sb.st_size, 1, fp);


    int count = 0;
    storage = malloc(sizeof(char) * strlen(file_contents));
    strcpy(storage, file_contents);

    char* ptr;
    char temp[50];
    ptr = strtok(file_contents, "ㅇ");
    FILE* text_file = NULL;

    while (ptr != NULL) {
//        printf("%s\n", ptr);
        strcpy(temp, ptr);
        opts->file_arr[count] = malloc(sizeof(char) * strlen(temp));
        strcpy(opts->file_arr[count], temp);
        ptr = strtok(NULL, "ㅇ");
        text_file = fopen(opts->file_arr[count], "w");
        if (!text_file) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        fwrite(ptr, strlen(ptr), 1, text_file);
        fclose(text_file);
        count++;
        ptr = strtok(NULL, "ㅇ");
    }

    opts->file_count = count;

//    for (int i = 0; i < opts->file_count; i++) {
//        printf("%s\n", opts->file_arr[i]);
//    }

    free(storage);
    free(file_contents);
    fclose(fp);
    remove_file(opts->directory);
}



void remove_file(char* directory) {
    chdir(directory);
    int remove_result = remove( "total.txt" );
    if( remove_result == -1 ) perror( "Failed to delete\n" );
}



#include "server.h"
#include "download.h"
#include "error.h"
#include "send.h"
#include "regex.h"




void download_file(struct options_server *opts) {

    ssize_t nbyte = 256;
    char received_content[BUF_SIZE];
    size_t filesize = 0, bufsize = 0;
    FILE *file = NULL;


    file = fopen("total.txt", "wb");
    bufsize = 256;

    while (nbyte != 0) {
        nbyte = recv(opts->client_socket, received_content, bufsize, 0);
        fwrite(received_content, sizeof(char), (unsigned long) nbyte, file);
    }
    fclose(file);
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



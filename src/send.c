#include "send.h"
#include "error.h"


void send_file(struct options *opts) {
    char buf[BUFSIZ];
    ssize_t server_confirm;
    char confirm[8];
    confirm[7] = '\0';
    char file_count[5];


    // Send server - # of .txt files
    sprintf(file_count, "%d", opts->file_count);
    strcat(file_count, "\n");
    write(opts->server_socket, file_count, strlen(file_count));
    while (TRUE) {
        server_confirm = read(opts->server_socket, confirm, sizeof(confirm));
        if (server_confirm == -1) {
            printf("server didn't get file count\n");
        }
        printf("SERVER GOT [ FILE COUNT ]: %s\n", confirm);
        break;
    }


    // Start to send file(s)
    for (int i = 0; i < opts->file_count; i++) {
        FILE *file;
        ssize_t file_size, current_size = 0;
        char file_size_string[10] = {0, };
        char file_name[20] = {0};

        strcpy(file_name, opts->file_arr[i]);
        strcat(file_name, "\n");
        // Send server - <filename>.txt
        write(opts->server_socket, file_name, strlen(file_name));
        server_confirm = read(opts->server_socket, confirm, sizeof(confirm));
        if (server_confirm == -1) {
            printf("server didn't get file name\n");
        }
        else {
            printf("SERVER GOT [ FILE NAME ]: %s\n", confirm);
        }


        // Send server - file size of <filename>.txt
        file = fopen(opts->file_arr[i], "rb");
        fseek(file, 0, SEEK_END);
        file_size = (int) ftell(file);
        fseek(file, 0, SEEK_SET);

        sprintf(file_size_string, "%d", file_size);
//    	printf("FILE SIZE = %s\n", file_size_string);
        strcat(file_size_string, "\n");
        write(opts->server_socket, file_size_string, strlen(file_size_string));
        server_confirm = read(opts->server_socket, confirm, sizeof(confirm));
        if (server_confirm == -1) {
            printf("server didn't get file size\n");
        }
        printf("SERVER GOT [ FILE SIZE ]: %s\n", confirm);


        // Send server - read <filename>.txt with 256 bytes and send buffer
        while(current_size != file_size) {
            size_t fp_size = fread(buf, 1, 256, file);
            current_size += fp_size;
            write(opts->server_socket, buf, (size_t) fp_size);
            if (current_size == file_size) {
                server_confirm = read(opts->server_socket, confirm, sizeof(confirm));
                if (server_confirm == -1) {
                    printf("server didn't get file content\n");
                }
                if (strcmp(confirm, "CONFIRM") == 0) {
                    printf("SERVER RECEIVED [ %s ] : %s\n", opts->file_arr[i], confirm);
                    break;
                }
            }
        }
        fclose(file);
    }
    close(opts->server_socket);
}

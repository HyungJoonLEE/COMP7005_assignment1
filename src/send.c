#include "send.h"
#include "error.h"


int get_confirm_message(struct options *opts) {
    char message[8];
    read(opts->server_socket, message, sizeof(message));
    if (strcmp(message, "confirm") == 0) {
        printf("GET CONFIRM\n");
        return TRUE;
    }
    return FALSE;
}



void send_number_of_file(struct options *opts) {
    int file_count = opts->file_count;
    char file_count_string[10];
    sprintf(file_count_string, "%d", file_count);
    write(opts->server_socket, file_count_string, strlen(file_count_string));

    while(1) {
        if (get_confirm_message(opts) == TRUE) {
            printf("hello\n");
            return;
        }
    }

}


void send_file(struct options *opts, size_t count) {
    char *buffer;
    char buf[BUFSIZ];
    char origin_size[8];
    unsigned long file_size, file_size_send, current_size = 0;
    FILE *file;

    buffer = malloc(count);
    if(buffer == NULL) fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);


    for (int i = 0; i < opts->file_count; i++) {
        file = fopen(opts->file_arr[i], "rb");
        fseek(file, 0, SEEK_END);
        file_size = (unsigned long) ftell(file);
        fseek(file, 0, SEEK_SET);
        file_size_send = htonl(file_size);
        send(opts->server_socket, &file_size_send, sizeof(file_size), 0);

        write(opts->server_socket, origin_size, strlen(origin_size));

        write(opts->server_socket, (char*)opts->file_arr[i], strlen(opts->file_arr[i]));


        while(current_size != file_size) {
            unsigned long fp_size = fread(buf, 1, 256, file);
            current_size += fp_size;
            send(opts->server_socket, buf, fp_size, 0);
        }
        fclose(file);
        write(opts->server_socket, "Send Complete", strlen("Send Complete"));
    }
    close(opts->server_socket);
}

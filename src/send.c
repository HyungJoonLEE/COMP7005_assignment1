#include "send.h"
#include "error.h"


void send_file(struct options *opts, size_t count) {
    char *buffer;
    char buf[BUFSIZ];

    buffer = malloc(count);
    if(buffer == NULL) fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);


    for (int i = 0; i < opts->file_count; i++) {
        FILE *file;
        unsigned long file_size, current_size = 0;

        file = fopen(opts->file_arr[i], "rb");
        fseek(file, 0, SEEK_END);
        file_size = (unsigned long) ftell(file);
        fseek(file, 0, SEEK_SET);

        while(current_size != file_size) {
            unsigned long fp_size = fread(buf, 1, 256, file);
            current_size += fp_size;
            send(opts->server_socket, buf, fp_size, 0);
        }
        write(opts->server_socket, "$$$$", strlen("$$$$"));
        fclose(file);
    }
    close(opts->server_socket);
}

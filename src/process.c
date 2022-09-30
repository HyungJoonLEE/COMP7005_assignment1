#include "process.h"
#include "error.h"
#include "common.h"
#include "download.h"


//void read_from_file(int server_socket, char* file_array[], int file_count, size_t count) {
void client_read_from_file(struct options *opts, size_t count) {
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
        /**
            Maybe I can send a name with '\0'  before read a file
            In server side, when it hits '\0' count++
                If count is even save as a file name
                If count is odd save as a content
         **/
        while(current_size != file_size) {
            unsigned long fp_size = fread(buf, 1, 256, file);
            current_size += fp_size;
            send(opts->server_socket, buf, fp_size, 0);
        }
        fclose(file);
    }
}

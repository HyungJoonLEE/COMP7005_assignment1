#include "server.h"
#include "download.h"
#include "error.h"
#include "process.h"



void download_file(struct options_server *opts) {

        struct sockaddr_in server_address;
        struct sockaddr_in client_address;
        socklen_t client_address_size;

        ssize_t read_len, file_read_len;
        char buf[MAXBUF];
        char file_name[MAXBUF];
        char dir_divider[2] = "/";
        ssize_t fd_write;


        client_address_size = sizeof(client_address);

        while(1) {
            memset(buf, 0x00, MAXBUF);
            opts->client_socket = accept(opts->server_socket, (struct sockaddr*)&client_address, &client_address_size);

            if(opts->client_socket == -1)
            {
                fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
            }
            printf("New Client Connect: %s\n", inet_ntoa(client_address.sin_addr));

            strcat(opts->directory, dir_divider);
            strcat(opts->directory, inet_ntoa(client_address.sin_addr));
            mkdirs(opts->directory, 0776);

            read_len = read(opts->fd_out, buf, MAXBUF);
            if(read_len > 0) {
                memset(file_name, 0x00, MAXBUF);
                strcpy(file_name, buf);
                printf("%s > %s\n", inet_ntoa(client_address.sin_addr), file_name);
            } else {
                close(opts->client_socket);
                break;
            }

            opts->fd_source = open(file_name, O_RDONLY);
            if(!opts->fd_source) {
                perror("file open error\n");
                break ;
            }
            while(1) {
                memset(buf, 0x00, MAXBUF);
                file_read_len = read(opts->fd_source, buf, MAXBUF);
                printf("\nread : %s",buf);
                fd_write = write(opts->client_socket, buf, MAXBUF);
                if(file_read_len == EOF | file_read_len == 0) {
                    printf("finish file\n");
                    break;
                }
            }

            close(opts->client_socket);
            close(opts->fd_source);
        }
    }



int mkdirs(const char *path, mode_t mode)
{
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

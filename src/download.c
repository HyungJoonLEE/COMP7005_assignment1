#include "server.h"
#include "download.h"
#include "error.h"
#include "process.h"



void download_file(struct options_server *opts) {
    ssize_t text_file_title;
    int text_file_title_fd;
    ssize_t nbyte = 256;
    char received_content[BUFSIZ];
    size_t filesize = 0, bufsize = 0;
    FILE *file = NULL;

    recv(opts->client_socket, received_content, strlen(received_content), 0);
    printf("this is received content: %s\n", received_content);
    file = fopen("hello.txt", "wb");

    //ntohl(filesize);
    //recv filesize

    //recv(clnt_sock, &filesize, sizeof(filesize), 0);
    //ntohl(filesize);
    //printf("file size = [%ld]\n", filesize);
    bufsize = 256;

    while (/*filesize != 0*/nbyte != 0) {
        //if(filesize < 256) bufsize = filesize;
        nbyte = recv(opts->client_socket, received_content, bufsize, 0);
        //printf("filesize:%ld nbyte: %d\n", filesize, nbyte);

        //filesize = filesize -nbyte;

        fwrite(received_content, sizeof(char), (unsigned long) nbyte, file);
        //nbyte = 0;
    }
}
//        client_address_size = sizeof(client_address);
//
//        while(1) {
//            memset(buf, 0x00, MAXBUF);
//
//            read_len = read(opts->fd_out, buf, MAXBUF);
//            if(read_len > 0) {
//                memset(file_name, 0x00, MAXBUF);
//                strcpy(file_name, buf);
//                printf("%s > %s\n", inet_ntoa(client_address.sin_addr), file_name);
//            } else {
//                close(opts->client_socket);
//                break;
//            }
//
//            opts->fd_source = open(file_name, O_RDONLY);
//            if(!opts->fd_source) {
//                perror("file open error\n");
//                break ;
//            }
//            while(1) {
//                memset(buf, 0x00, MAXBUF);
//                file_read_len = read(opts->fd_source, buf, MAXBUF);
//                printf("\nread : %s",buf);
//                fd_write = write(opts->client_socket, buf, MAXBUF);
//                if(file_read_len == EOF | file_read_len == 0) {
//                    printf("finish file\n");
//                    break;
//                }
//            }
//
//            close(opts->client_socket);
//            close(opts->fd_source);
//        }
//    }



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

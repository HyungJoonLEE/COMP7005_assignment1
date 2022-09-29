#include "server.h"
#include "download.h"
#include "error.h"


void download_file(struct options *opts) {
    if(opts->file_name && opts->ip_in)
    {
        fatal_message(__FILE__, __func__ , __LINE__, "Can't pass -i and a filename", 2);
    }

    if(opts->file_name)
    {
        opts->fd_in = open(opts->file_name, O_RDONLY);

        if(opts->fd_in == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }
    }

    if(opts->ip_in)
    {
        struct sockaddr_in server_address;
        struct sockaddr_in client_address;
        socklen_t client_address_size;
        int result;
        int option;
        ssize_t read_len, file_read_len;
        char buf[MAXBUF];
        char file_name[MAXBUF];
        ssize_t fd_write;

        opts->fd_in2 = socket(AF_INET, SOCK_STREAM, 0);

        if(opts->fd_in2 == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(opts->port_in);
        server_address.sin_addr.s_addr = inet_addr(opts->ip_in);

        if(server_address.sin_addr.s_addr ==  (in_addr_t)-1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        option = 1;
        setsockopt(opts->fd_in2, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

        result = bind(opts->fd_in2, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));

        if(result == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        result = listen(opts->fd_in2, BACKLOG);

        if(result == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        client_address_size = sizeof(client_address);

        while(1) {
            memset(buf, 0x00, MAXBUF);
            opts->client_socket = accept(opts->fd_in2, (struct sockaddr*)&client_address, &client_address_size);

            if(opts->client_socket == -1)
            {
                fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
            }
            printf("New Client Connect: %s\n", inet_ntoa(client_address.sin_addr));

            read_len = read(opts->client_socket, buf, MAXBUF);
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
            perror("file open error : ");
            break ;
        }
        printf("before while\n");
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
}
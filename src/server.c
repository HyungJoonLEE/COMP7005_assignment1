#include "conversion.h"
#include "copy.h"
#include "error.h"
#include "server.h"
#include "download.h"
#include "send.h"
//#include <linux/if.h>
#include <sys/time.h>
#include <net/if.h>





#define BUF_SIZE 1024
#define BACKLOG 5

int main(int argc, char *argv[])
{
    struct options_server opts;

    options_init_server(&opts);
    parse_arguments_server(argc, argv, &opts);
    options_process_server(&opts);
//    copy(opts.client_socket, opts.fd_out, BUF_SIZE);
//    download_file(&opts);
//    save_file(&opts);
    cleanup_server(&opts);
    return EXIT_SUCCESS;
}


static void options_init_server(struct options_server *opts)
{
    memset(opts, 0, sizeof(struct options));
    opts->fd_in    = STDIN_FILENO;
    opts->port_in  = DEFAULT_PORT;
    strcpy(opts->directory, DEFAULT_DIRECTORY);
    opts->port_in  = DEFAULT_PORT;
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "en0", IF_NAMESIZE);
    ioctl(fd, SIOCGIFADDR, &ifr);
    printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    memset(opts, 0, sizeof(struct options_server));

    // DEFAULT SETTINGS FOR OPTION
    opts->ip_in = malloc(sizeof(char) * strlen(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr)));
    strcpy(opts->ip_in,  inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    opts->fd_in   = STDOUT_FILENO;
    strcpy(opts->directory, DEFAULT_DIRECTORY);
    printf("default directory: %s\n", opts->directory);
    opts->port_in  = DEFAULT_PORT;
}


static void parse_arguments_server(int argc, char *argv[], struct options_server *opts)
{
    int c;

    while((c = getopt(argc, argv, ":i:d:p:")) != -1)   // NOLINT(concurrency-mt-unsafe)
    {
        switch(c)
        {
            case 'i':
            {
                opts->ip_in = optarg;
                break;
            }
            case 'd':
            {
                strcpy(opts->directory, optarg);
                strcpy(opts->origin_directory, optarg);
                break;
            }
            case 'p':
            {
                opts->port_in = parse_port(optarg, 10); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
                break;
            }
            case ':':
            {
                fatal_message(__FILE__, __func__ , __LINE__, "\"Option requires an operand\"", 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
                break;
            }
            case '?':
            {
                fatal_message(__FILE__, __func__ , __LINE__, "Unknown", 6); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            }
            default:
            {
                assert("should not get here");
            };
        }
    }
}


static void options_process_server(struct options_server *opts)
{
    char* message="You are connected to server\n";

    if(opts->ip_in)
    {
        struct sockaddr_in server_address;
        struct sockaddr_in client_address;
        socklen_t client_address_size;
        int result;
        int option = TRUE;
        char dir_divider[2] = "/";
        fd_set readfds;
        int max_sd, sd, activity, new_socket, valread;
        char buffer[1024];

        opts->server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if(opts->server_socket == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        for (int i = 0; i < 5; i++) {
            opts->client_socket[i] = 0;
        }

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(opts->port_in);
        server_address.sin_addr.s_addr = inet_addr(opts->ip_in);

        if(server_address.sin_addr.s_addr ==  (in_addr_t)-1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        option = 1;
        setsockopt(opts->server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

        result = bind(opts->server_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));

        if(result == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        result = listen(opts->server_socket, BACKLOG);

        if(result == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }

        client_address_size = sizeof(client_address);

        while(TRUE) {
            FD_ZERO((&readfds));
            FD_SET(opts->server_socket, &readfds);
            max_sd = opts->server_socket;

            for (int i = 0; i < 5;/*max client*/ i++) {
                sd = opts->client_socket[i];
                if (sd > 0) FD_SET(sd, &readfds);
                if (sd > max_sd) max_sd = sd;
            }
            activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
            if (activity < 0) printf("select error\n");

            if (FD_ISSET(opts->server_socket, &readfds)) {
                if ((new_socket = accept(opts->server_socket, (struct sockaddr *) &client_address,
                                         &client_address_size)) < 0) {
                    perror("accept: ");
                    exit(EXIT_FAILURE);
                }

                printf("===== [New Client Connect] ======\n"
                       "Socket fd : %d\n"
                       "ip : %s\n"
                       "port : %d\n", new_socket, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                strcpy(opts->directory, opts->origin_directory);
                strcat(opts->directory, dir_divider);
                strcat(opts->directory, inet_ntoa(client_address.sin_addr));
                mkdirs(opts->directory, 0776);
                if(chdir(opts->directory) == 0) printf("Directory changed => %s\n", opts->directory);

                if (send(new_socket, message, strlen(message), 0) != strlen(message)) perror("send : ");

                for (int i = 0; i < 5; i++) {
                    if (opts->client_socket[i] == 0) {
                        opts->client_socket[i] = new_socket;
                        opts->active_sd = new_socket;
                        printf("Successfully added in client socket array: [%d]\n", i);
                        break;
                    }
                }
            }
            download_file(opts);
            save_file(opts);
            chdir(opts->origin_directory);

            for (int i = 0; i < 5; i++) {
                sd = opts->client_socket[i];
                if (FD_ISSET(sd, &readfds)) {
                    if ((valread = read(sd, buffer, 1024)) == 0) {
                        getpeername(sd, (struct sockaddr *) &client_address, &client_address_size);
                        printf("DISCONNECTED [ ip : %s ]\n", inet_ntoa(client_address.sin_addr));
                        close(sd);
                        opts->client_socket[i] = 0;
                    } else {
                        buffer[valread] = '\0';
                    }
                }
            }

//            opts->client_socket = accept(opts->server_socket, (struct sockaddr*)&client_address, &client_address_size);
//
//            if(opts->client_socket == -1)
//            {
//                fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
//            }
//            write(opts->client_socket, message, strlen(message));
//
//            printf("===== [New Client Connect] : %s ======\n", inet_ntoa(client_address.sin_addr));
        }
    }
}


static void cleanup_server(const struct options_server *opts)
{
    for (int i = 0; i < opts->file_count; i++) {
        free(opts->file_arr[i]);
    }
//    if(opts->file_name)
//    {
//        close(opts->fd_in);
//    }
    if(opts->ip_in)
    {
        free(opts->ip_in);
        close(opts->fd_in);
        close(opts->server_socket);
    }
}

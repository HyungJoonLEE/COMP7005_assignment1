#include "conversion.h"
#include "copy.h"
#include "error.h"
#include "server.h"
#include "download.h"
#include "send.h"

#define BUF_SIZE 1024
#define BACKLOG 5

int main(int argc, char *argv[])
{
    struct options_server opts;

    options_init_server(&opts);
    parse_arguments_server(argc, argv, &opts);
    options_process_server(&opts);
//    copy(opts.client_socket, opts.fd_out, BUF_SIZE);

    download_file(&opts);
    get_text_name(&opts);
    create_name_file(&opts);
    cleanup_server(&opts);
    return EXIT_SUCCESS;
}


static void options_init_server(struct options_server *opts)
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    snprintf(ifr.ifr_name, IFNAMSIZ, "en0");
    ioctl(fd, SIOCGIFADDR, &ifr);
    printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    memset(opts, 0, sizeof(struct options_server));

    // DEFAULT SETTINGS FOR OPTION
    opts->ip_in   = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
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
                printf("changed directory: %s\n", opts->directory);
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

    if(optind < argc)
    {
        opts->file_name = argv[optind];
    }
}


static void options_process_server(struct options_server *opts)
{
    char* message="You are connected to server\n";
    if(opts->file_name && opts->ip_in)
    {
        fatal_message(__FILE__, __func__ , __LINE__, "Can't pass -i and a filename", 2);
    }

//    if(opts->file_name)
//    {
//        opts->fd_in = open(opts->file_name, O_RDONLY);
//
//        if(opts->fd_in == -1)
//        {
//            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
//        }
//    }

    if(opts->ip_in)
    {
        struct sockaddr_in server_address;
        struct sockaddr_in client_address;
        socklen_t client_address_size;
        int result;
        int option;
        char dir_divider[2] = "/";

        opts->server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if(opts->server_socket == -1)
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

        opts->client_socket = accept(opts->server_socket, (struct sockaddr*)&client_address, &client_address_size);

//        if(opts->fd_in == -1 || opts->client_socket == -1)
        if(opts->client_socket == -1)
        {
            fatal_errno(__FILE__, __func__ , __LINE__, errno, 2);
        }
        write(opts->client_socket, message, strlen(message));

        printf("New Client Connect: %s\n", inet_ntoa(client_address.sin_addr));

        strcat(opts->directory, dir_divider);
        strcat(opts->directory, inet_ntoa(client_address.sin_addr));
        mkdirs(opts->directory, 0776);
        if(chdir(opts->directory) == 0) printf("Directory changed to %s\n", opts->directory);
    }
}


static void cleanup_server(const struct options_server *opts)
{
    for (int i = 0; i < opts->file_count; i++) {
        free(opts->file_arr[i]);
    }
    if(opts->file_name)
    {
        close(opts->fd_in);
    }
    else if(opts->ip_in)
    {
        close(opts->fd_in);
        close(opts->server_socket);
    }
}

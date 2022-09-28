#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <dc_application/command_line.h>
#include <dc_application/config.h>
#include <dc_application/options.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>
#include <getopt.h>
#include <inttypes.h>


#include "client.h"
#include "common.h"

void *listeningThread(void *args);

uint8_t cmd_val(const char *cmd);

uint16_t current_channel;

int main(int argc, char *argv[]) {
    dc_posix_tracer tracer;
    dc_error_reporter reporter;
    struct dc_posix_env env;
    struct dc_error err;
    struct dc_application_info *info;
    int ret_val;

    tracer = NULL;
    dc_posix_env_init(&env, tracer);
    reporter = dc_error_default_error_reporter;
    dc_error_init(&err, reporter);
    info = dc_application_info_create(&env, &err, "COMP7005 ASSIGN1");
    ret_val = dc_application_run(&env, &err, info, create_settings, destroy_settings, run, dc_default_create_lifecycle,
                                 dc_default_destroy_lifecycle,
                                 "~/.dcecho.conf",
                                 argc, argv);
    dc_application_info_destroy(&env, &info);
    dc_error_reset(&err);

    return ret_val;
}

static struct dc_application_settings *create_settings(const struct dc_posix_env *env, struct dc_error *err) {
    static const char *default_hostname = "localhost";
    static const uint16_t default_port = DEFAULT_PORT;
    struct application_settings *settings;


    settings = dc_malloc(env, err, sizeof(struct application_settings));

    if (settings == NULL) {
        return NULL;
    }

    settings->opts.parent.config_path = dc_setting_path_create(env, err);
    settings->hostname = dc_setting_string_create(env, err);
    settings->port = dc_setting_uint16_create(env, err);
    settings->filename = dc_setting_string_create(env, err);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement"
    struct options opts[] =
            {
                    {(struct dc_setting *) settings->opts.parent.config_path, dc_options_set_path,   "config", required_argument, 'c', "CONFIG", dc_string_from_string, NULL,   dc_string_from_config, NULL},
                    {(struct dc_setting *) settings->hostname,                dc_options_set_string, "host",   required_argument, 's', "HOST",   dc_string_from_string, "host", dc_string_from_config, default_hostname},
                    {(struct dc_setting *) settings->filename,                dc_options_set_string, "filename",   required_argument, 'f', "FILENAME",   dc_string_from_string, "filename", dc_string_from_config, "foo.txt"},
                    {(struct dc_setting *) settings->port,                    dc_options_set_uint16, "port",   required_argument, 'p', "PORT",   dc_uint16_from_string, "port", dc_uint16_from_config, &default_port},
            };
#pragma GCC diagnostic pop

    // note the trick here - we use calloc and add 1 to ensure the last line is all 0/NULL
    settings->opts.opts = dc_calloc(env, err, (sizeof(opts) / sizeof(struct options)) + 1, sizeof(struct options));
    dc_memcpy(env, settings->opts.opts, opts, sizeof(opts));
    settings->opts.flags = "c:s:p:f:";
    settings->opts.env_prefix = "CPT_CHAT_";

    return (struct dc_application_settings *) settings;
}

static int destroy_settings(const struct dc_posix_env *env, __attribute__ ((unused)) struct dc_error *err,
                            struct dc_application_settings **psettings) {
    struct application_settings *app_settings;

    app_settings = (struct application_settings *) *psettings;
    dc_setting_string_destroy(env, &app_settings->hostname);
    dc_setting_string_destroy(env, &app_settings->filename);
    dc_setting_uint16_destroy(env, &app_settings->port);
    dc_free(env, app_settings->opts.opts, app_settings->opts.opts_size);
    dc_free(env, app_settings, sizeof(struct application_settings));

    if (env->null_free) {
        *psettings = NULL;
    }

    return 0;
}


static int run(const struct dc_posix_env *env, __attribute__ ((unused)) struct dc_error *err,
               struct dc_application_settings *settings) {
    struct application_settings *app_settings;
    const char *hostname;
    const char *filename;
    in_port_t port;
    int ret_val;
    int sd = -1, rc;
    char buffer[BUFFER_LENGTH];
    char server[BUFSIZ];
    struct sockaddr_in6 serveraddr;
    struct addrinfo hints, *res;
    pthread_t thread_id;
    size_t size_buf;
    uint8_t *buff;

    app_settings = (struct application_settings *) settings;
    hostname     = dc_setting_string_get(env, app_settings->hostname);
    port         = dc_setting_uint16_get(env, app_settings->port);
    filename     = dc_setting_string_get(env, app_settings->filename);
    ret_val      = 0;

    do
    {
        sd = socket(AF_INET6, SOCK_STREAM, 0);
        if (sd < 0)
        {
            perror("socket() failed");
            break;
        }

        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin6_family = AF_INET6;
        serveraddr.sin6_port = htons(port);
        rc = inet_pton(AF_INET6, hostname, &serveraddr.sin6_addr.s6_addr);

        if (rc != 1)
        {
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET6;
            hints.ai_flags = AI_V4MAPPED;
            rc = getaddrinfo(hostname, NULL, &hints, &res);
            if (rc != 0)
            {
                printf("Host not found! (%s)\n", hostname);
                perror("getaddrinfo() failed\n");
                break;
            }

            memcpy(&serveraddr.sin6_addr,
                   (&((struct sockaddr_in6 *) (res->ai_addr))->sin6_addr),
                   sizeof(serveraddr.sin6_addr));

            freeaddrinfo(res);
        }

        rc = connect(sd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
        if (rc < 0) {
            perror("connect() failed");
            break;
        }

        // An infinite loop that listens for user's keyboard and send the message
        while (TRUE)
        {
            // Take input from client and send it to the server
            char message[MSG_MAX_LEN];

            ssize_t message_len;
            message_len = read(STDIN_FILENO, message, MSG_MAX_LEN);
            message[message_len] = '\0';
            char *message_copy = strdup(message);

            char *parse_message;
            parse_message = strtok(message_copy, " ");
            parse_message = strtok(NULL, "\n");
            free(message_copy);

            buff = calloc(size_buf, sizeof(uint8_t));

            rc = send(sd, buff, size_buf, 0);
            if (rc < 0) {
                perror("send() failed");
                break;
            }
            free(buff);
        }
        close(sd);
    } while (FALSE);

    /* Close down any open socket descriptors                              */
    if (sd != -1) close(sd);
    return ret_val;
}


//
// Created by Hyung Joon Lee on 2022-09-27.
//
#ifndef ASSIGN1_SERVER_H
#define ASSIGN1_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <dc_application/command_line.h>
#include <dc_application/options.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>
#include <dc_application/config.h>
#include <dc_posix/dc_inttypes.h>
#include <getopt.h>
#include <inttypes.h>

#define DEFAULT_DIRECTORY " ~/server/Downloads"


struct application_settings
{
    struct dc_opt_settings opts;
    struct dc_setting_uint16 *port;
    struct dc_setting_string *directory;
};


static struct dc_application_settings *create_settings(const struct dc_posix_env *env, struct dc_error *err);
static int destroy_settings(const struct dc_posix_env *env,
                            struct dc_error *err,
                            struct dc_application_settings **psettings);
static int run(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *settings);
static void error_reporter(const struct dc_error *err);
static void trace_reporter(const struct dc_posix_env *env,
                           const char *file_name,
                           const char *function_name,
                           size_t line_number);


#endif //ASSIGN1_SERVER_H

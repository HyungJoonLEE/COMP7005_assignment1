//
// Created by Hyung Joon Lee on 2022-09-27.
//
#ifndef ASSIGN1_SERVER_H
#define ASSIGN1_SERVER_H

#define DEFAULT_PORT 8000
#define DEFAULT_DIRECTORY " ~/server"


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

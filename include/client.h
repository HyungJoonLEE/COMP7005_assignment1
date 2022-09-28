//
// Created by Hyung Joon Lee on 2022-09-28.
//
#ifndef ASSIGNMENT1_CLIENT_H
#define ASSIGNMENT1_CLIENT_H


#include <dc_application/options.h>

struct application_settings
{
    struct dc_opt_settings opts;
    struct dc_setting_string *hostname;
    struct dc_setting_uint16 *port;
    struct dc_setting_string *filename
};


static struct dc_application_settings *create_settings(const struct dc_posix_env *env, struct dc_error *err);

static int
destroy_settings(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings **psettings);

static int run(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *settings);

#endif //ASSIGNMENT1_CLIENT_H

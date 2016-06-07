#ifndef CHECK_H
#define CHECK_H

#include "duv.h"

duk_ret_t duv_new_check(duk_context *ctx);
duk_ret_t duv_check_start(duk_context *ctx);
duk_ret_t duv_check_stop(duk_context *ctx);

#endif

#ifndef PREPARE_H
#define PREPARE_H

#include "duv.h"

duk_ret_t duv_new_prepare(duk_context *ctx);
duk_ret_t duv_prepare_start(duk_context *ctx);
duk_ret_t duv_prepare_stop(duk_context *ctx);

#endif

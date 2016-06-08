#ifndef IDLE_H
#define IDLE_H

#include "duv.h"

duk_ret_t duv_new_idle(duk_context *ctx);
duk_ret_t duv_idle_start(duk_context *ctx);
duk_ret_t duv_idle_stop(duk_context *ctx);

#endif

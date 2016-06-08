#ifndef ASYNC_H
#define ASYNC_H

#include "duv.h"

duk_ret_t duv_new_async(duk_context *ctx);
duk_ret_t duv_async_send(duk_context *ctx);

#endif

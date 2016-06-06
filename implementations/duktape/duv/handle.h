#ifndef HANDLE_H
#define HANDLE_H

#include "duv.h"

duk_ret_t duv_tostring(duk_context *ctx);
duk_ret_t duv_close(duk_context *ctx);
duk_ret_t duv_is_active(duk_context *ctx);
duk_ret_t duv_is_closing(duk_context *ctx);
duk_ret_t duv_ref(duk_context *ctx);
duk_ret_t duv_unref(duk_context *ctx);
duk_ret_t duv_has_ref(duk_context *ctx);


#endif

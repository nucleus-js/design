#ifndef ENV_H
#define ENV_H

#include "duktape-releases/src/duktape.h"
duk_ret_t env_keys(duk_context *ctx);
duk_ret_t env_get(duk_context *ctx);
duk_ret_t env_set(duk_context *ctx);
duk_ret_t env_unset(duk_context *ctx);

#endif

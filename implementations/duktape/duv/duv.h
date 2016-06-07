#ifndef DUV_H
#define DUV_H

#include "../../../deps/libuv/include/uv.h"
#include "../duktape-releases/src/duktape.h"

typedef enum {
  DUV_TIMER,
  DUV_PREPARE,
  DUV_CHECK,
  DUV_TCP,
  DUV_PIPE,
} duv_type_t;

typedef enum {
  DUV_HANDLE_MASK =
    1 << DUV_TIMER |
    1 << DUV_PREPARE |
    1 << DUV_CHECK |
    1 << DUV_TCP |
    1 << DUV_PIPE,
  DUV_TIMER_MASK = 1 << DUV_TIMER,
  DUV_PREPARE_MASK = 1 << DUV_PREPARE,
  DUV_CHECK_MASK = 1 << DUV_CHECK,
  DUV_STREAM_MASK = 1 << DUV_TCP | 1 << DUV_PIPE,
  DUV_TCP_MASK = 1 << DUV_TCP,
  DUV_PIPE_MASK = 1 << DUV_PIPE,
} duv_type_mask_t;

// Create a new duv instance with own loop, and push onto duktape context.
duk_ret_t duv_push_module(duk_context *ctx);

#endif

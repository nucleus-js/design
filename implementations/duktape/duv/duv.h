#ifndef DUV_H
#define DUV_H

#include "../../../deps/libuv/include/uv.h"
#include "../duktape-releases/src/duktape.h"

typedef enum {
  DUV_TIMER,
  DUV_PREPARE,
  DUV_CHECK,
  DUV_IDLE,
  DUV_ASYNC,
  DUV_POLL,
  DUV_SIGNAL,
  DUV_PROCESS,
  DUV_TCP,
  DUV_PIPE,
  DUV_TTY,
  DUV_UDP,
  DUV_FS_EVENT,
  DUV_FS_POLL,
} duv_type_t;

typedef enum {
  DUV_HANDLE_MASK =
    1 << DUV_TIMER |
    1 << DUV_PREPARE |
    1 << DUV_CHECK |
    1 << DUV_IDLE |
    1 << DUV_ASYNC |
    1 << DUV_POLL |
    1 << DUV_SIGNAL |
    1 << DUV_PROCESS |
    1 << DUV_TCP |
    1 << DUV_PIPE |
    1 << DUV_TTY |
    1 << DUV_UDP |
    1 << DUV_FS_EVENT |
    1 << DUV_FS_POLL,
  DUV_TIMER_MASK = 1 << DUV_TIMER,
  DUV_PREPARE_MASK = 1 << DUV_PREPARE,
  DUV_CHECK_MASK = 1 << DUV_CHECK,
  DUV_IDLE_MASK = 1 << DUV_IDLE,
  DUV_ASYNC_MASK = 1 << DUV_ASYNC,
  DUV_POLL_MASK = 1 << DUV_POLL,
  DUV_SIGNAL_MASK = 1 << DUV_SIGNAL,
  DUV_PROCESS_MASK = 1 << DUV_PROCESS,
  DUV_STREAM_MASK = 1 << DUV_TCP | 1 << DUV_PIPE | 1 << DUV_TTY,
  DUV_TCP_MASK = 1 << DUV_TCP,
  DUV_PIPE_MASK = 1 << DUV_PIPE,
  DUV_TTY_MASK = 1 << DUV_TTY,
  DUV_UDP_MASK = 1 << DUV_UDP,
  DUV_FS_EVENT_MASK = 1 << DUV_FS_EVENT,
  DUV_FS_POLL_MASK = 1 << DUV_FS_POLL,
} duv_type_mask_t;

// Create a new duv instance with own loop, and push onto duktape context.
duk_ret_t duv_push_module(duk_context *ctx);

#include "utils.h"
#include "callbacks.h"

#endif

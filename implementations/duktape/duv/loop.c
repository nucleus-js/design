#include "loop.h"

duk_ret_t duv_run(duk_context *ctx) {
  int ret = uv_run(duv_loop(ctx), UV_RUN_DEFAULT);
  if (ret < 0) {
    duv_error(ctx, ret);
  }
  return 0;
}

static void duv_walk_cb(uv_handle_t *handle, void* c) {
  duk_context *ctx = c;
  duk_dup(ctx, 0);
  duv_push_handle(ctx, handle);
  duk_call(ctx, 1);
  duk_pop(ctx);
}

duk_ret_t duv_walk(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", duk_is_function},
    {0,0}
  });
  uv_walk(duv_loop(ctx), duv_walk_cb, ctx);
  return 0;
}

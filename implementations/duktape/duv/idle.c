#include "idle.h"

duk_ret_t duv_new_idle(duk_context *ctx) {
  uv_idle_t *idle = duk_push_fixed_buffer(ctx, sizeof(uv_idle_t));
  duv_check(ctx, uv_idle_init(duv_loop(ctx), idle));
  duv_setup_handle(ctx, (uv_handle_t*)idle, DUV_IDLE);
  return 1;
}

duk_ret_t duv_idle_start(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", duk_is_function},
    {0,0}
  });
  uv_idle_t *idle = duv_require_this_handle(ctx, DUV_IDLE_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-idle");
  duv_check(ctx, uv_idle_start(idle, duv_on_idle));
  return 0;
}

duk_ret_t duv_idle_stop(duk_context *ctx) {
  uv_idle_t *idle = duv_require_this_handle(ctx, DUV_IDLE_MASK);
  duk_del_prop_string(ctx, -1, "\xffon-idle");
  duv_check(ctx, uv_idle_stop(idle));
  return 0;
}

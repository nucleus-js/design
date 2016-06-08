#include "timer.h"

duk_ret_t duv_new_timer(duk_context *ctx) {
  uv_timer_t *timer = duk_push_fixed_buffer(ctx, sizeof(uv_timer_t));
  duv_check(ctx, uv_timer_init(duv_loop(ctx), timer));
  duv_setup_handle(ctx, (uv_handle_t*)timer, DUV_TIMER);
  return 1;
}

duk_ret_t duv_timer_start(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"timeout", duk_is_number},
    {"repeat", duk_is_number},
    {"callback", duk_is_function},
    {0,0}
  });
  uv_timer_t *timer = duv_require_this_handle(ctx, DUV_TIMER_MASK);
  int timeout = duk_get_int(ctx, 1);
  int repeat = duk_get_int(ctx, 2);
  duk_put_prop_string(ctx, 0, "\xffon-timeout");
  duv_check(ctx, uv_timer_start(timer, duv_on_timeout, timeout, repeat));
  return 0;
}

duk_ret_t duv_timer_stop(duk_context *ctx) {
  uv_timer_t *timer = duv_require_this_handle(ctx, DUV_TIMER_MASK);
  duk_del_prop_string(ctx, -1, "\xffon-timeout");
  duv_check(ctx, uv_timer_stop(timer));
  return 0;
}

duk_ret_t duv_timer_again(duk_context *ctx) {
  uv_timer_t *timer = duv_require_this_handle(ctx, DUV_TIMER_MASK);
  duv_check(ctx, uv_timer_again(timer));
  return 0;
}

duk_ret_t duv_timer_set_repeat(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"repeat", duk_is_number},
    {0,0}
  });
  uv_timer_t *timer = duv_require_this_handle(ctx, DUV_TIMER_MASK);
  uint64_t repeat = duk_get_int(ctx, 1);
  uv_timer_set_repeat(timer, repeat);
  return 0;
}

duk_ret_t duv_timer_get_repeat(duk_context *ctx) {
  uv_timer_t *timer = duv_require_this_handle(ctx, DUV_TIMER_MASK);
  duk_push_int(ctx, uv_timer_get_repeat(timer));
  return 1;
}

#include "check.h"

duk_ret_t duv_new_check(duk_context *ctx) {
  uv_check_t *check = duk_push_fixed_buffer(ctx, sizeof(uv_check_t));
  duv_check(ctx, uv_check_init(duv_loop(ctx), check));
  duv_setup_handle(ctx, (uv_handle_t*)check, DUV_CHECK);
  return 1;
}

duk_ret_t duv_check_start(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", duk_is_function},
    {0,0}
  });
  uv_check_t *check = duv_require_this_handle(ctx, DUV_CHECK_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-check");
  duv_check(ctx, uv_check_start(check, duv_on_check));
  return 0;
}

duk_ret_t duv_check_stop(duk_context *ctx) {
  uv_check_t *check = duv_require_this_handle(ctx, DUV_CHECK_MASK);
  duk_del_prop_string(ctx, -1, "\xffon-check");
  duv_check(ctx, uv_check_stop(check));
  return 0;
}

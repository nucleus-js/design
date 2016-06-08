#include "prepare.h"

duk_ret_t duv_new_prepare(duk_context *ctx) {
  uv_prepare_t *prepare = duk_push_fixed_buffer(ctx, sizeof(uv_prepare_t));
  duv_check(ctx, uv_prepare_init(duv_loop(ctx), prepare));
  duv_setup_handle(ctx, (uv_handle_t*)prepare, DUV_PREPARE);
  return 1;
}

duk_ret_t duv_prepare_start(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", duk_is_function},
    {0,0}
  });
  uv_prepare_t *prepare = duv_require_this_handle(ctx, DUV_PREPARE_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-prepare");
  duv_check(ctx, uv_prepare_start(prepare, duv_on_prepare));
  return 0;
}

duk_ret_t duv_prepare_stop(duk_context *ctx) {
  uv_prepare_t *prepare = duv_require_this_handle(ctx, DUV_PREPARE_MASK);
  duk_del_prop_string(ctx, -1, "\xffon-prepare");
  duv_check(ctx, uv_prepare_stop(prepare));
  return 0;
}

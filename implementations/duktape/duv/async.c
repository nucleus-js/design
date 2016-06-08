#include "async.h"

duk_ret_t duv_new_async(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", duk_is_function},
    {0,0}
  });
  uv_async_t *async = duk_push_fixed_buffer(ctx, sizeof(uv_async_t));
  duv_check(ctx, uv_async_init(duv_loop(ctx), async, duv_on_async));
  duv_setup_handle(ctx, (uv_handle_t*)async, DUV_ASYNC);
  duk_insert(ctx, 0);
  duk_put_prop_string(ctx, 0, "\xffon-async");
  return 1;
}

duk_ret_t duv_async_send(duk_context *ctx) {
  uv_async_t *async = duv_require_this_handle(ctx, DUV_ASYNC_MASK);
  duv_check(ctx, uv_async_send(async));
  return 0;
}

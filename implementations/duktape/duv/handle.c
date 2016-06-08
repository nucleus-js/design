#include "handle.h"

duk_ret_t duv_tostring(duk_context *ctx) {
  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, "\xffuv-type");
  if (duk_is_undefined(ctx, -1)) return 0;
  duk_get_prop_string(ctx, -2, "\xffuv-data");
  const char* type = duv_type_to_string(duk_get_int(ctx, -2));
  void* data = duk_get_buffer(ctx, -1, 0);
  duk_pop_3(ctx);
  duk_push_sprintf(ctx, "[%s %p]", type, data);
  return 1;
}

duk_ret_t duv_close(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_handle_t *handle = duv_require_this_handle(ctx, DUV_HANDLE_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-close");
  uv_close(handle, duv_on_close);
  return 0;
}


duk_ret_t duv_is_active(duk_context *ctx) {
  uv_handle_t *handle = duv_require_this_handle(ctx, DUV_HANDLE_MASK);
  duk_push_boolean(ctx, uv_is_active(handle));
  return 1;
}

duk_ret_t duv_is_closing(duk_context *ctx) {
  uv_handle_t *handle = duv_require_this_handle(ctx, DUV_HANDLE_MASK);
  duk_push_boolean(ctx, uv_is_closing(handle));
  return 1;
}

duk_ret_t duv_ref(duk_context *ctx) {
  uv_handle_t *handle = duv_require_this_handle(ctx, DUV_HANDLE_MASK);
  uv_ref(handle);
  return 0;
}

duk_ret_t duv_unref(duk_context *ctx) {
  uv_handle_t *handle = duv_require_this_handle(ctx, DUV_HANDLE_MASK);
  uv_unref(handle);
  return 0;
}

duk_ret_t duv_has_ref(duk_context *ctx) {
  uv_handle_t *handle = duv_require_this_handle(ctx, DUV_HANDLE_MASK);
  duk_push_boolean(ctx, uv_has_ref(handle));
  return 1;
}

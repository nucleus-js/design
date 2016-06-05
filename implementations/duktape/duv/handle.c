#include "handle.h"
#include "utils.h"

void duv_on_close(uv_handle_t *handle) {
  duk_context *ctx = handle->data;
  duv_push_handle(ctx, handle);
  duk_get_prop_string(ctx, -1, "\xffon-close");
  duk_remove(ctx, -2);
  if (duk_is_function(ctx, -1)) {
    duk_call(ctx, 0);
  }
  duk_pop(ctx);
}

duk_ret_t duv_close(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_handle_t *handle = duv_require_this_handle(ctx, DUV_HANDLE_MASK);
  duk_put_prop_string(ctx, -2, "\xffon-close");
  uv_close(handle, duv_on_close);
  return 0;
}

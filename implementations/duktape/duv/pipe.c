#include "pipe.h"

duk_ret_t duv_new_pipe(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"ipc", duk_is_boolean},
    {0,0}
  });
  uv_pipe_t *pipe = duk_push_fixed_buffer(ctx, sizeof(uv_pipe_t));
  duv_check(ctx, uv_pipe_init(duv_loop(ctx), pipe, duk_get_int(ctx, 0)));
  duv_setup_handle(ctx, (uv_handle_t*)pipe, DUV_PIPE);
  return 1;
}

duk_ret_t duv_pipe_open(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"fd", duk_is_number},
    {0,0}
  });
  uv_pipe_t *pipe = duv_require_this_handle(ctx, DUV_PIPE_MASK);
  duv_check(ctx, uv_pipe_open(pipe,
    duk_get_int(ctx, 1)
  ));
  return 0;
}

duk_ret_t duv_pipe_bind(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"name", duk_is_string},
    {0,0}
  });
  uv_pipe_t *pipe = duv_require_this_handle(ctx, DUV_PIPE_MASK);
  duv_check(ctx, uv_pipe_bind(pipe,
    duk_get_string(ctx, 1)
  ));
  return 0;
}

duk_ret_t duv_pipe_connect(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"name", duk_is_string},
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_pipe_t *pipe = duv_require_this_handle(ctx, DUV_TCP_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-connect");
  uv_connect_t *req = duk_push_fixed_buffer(ctx, sizeof(*req));
  duk_put_prop_string(ctx, 0, "\xffreq-connect");
  uv_pipe_connect(req, pipe, duk_get_string(ctx, 1), duv_on_connect);
  return 0;
}

duk_ret_t duv_pipe_getsockname(duk_context *ctx) {
  uv_pipe_t *pipe = duv_require_this_handle(ctx, DUV_TCP_MASK);
  size_t len = 2*PATH_MAX;
  char buf[2*PATH_MAX];
  duv_check(ctx, uv_pipe_getsockname(pipe, buf, &len));
  duk_push_lstring(ctx, buf, len);
  return 1;
}

duk_ret_t duv_pipe_getpeername(duk_context *ctx) {
  uv_pipe_t *pipe = duv_require_this_handle(ctx, DUV_TCP_MASK);
  size_t len = 2*PATH_MAX;
  char buf[2*PATH_MAX];
  duv_check(ctx, uv_pipe_getpeername(pipe, buf, &len));
  duk_push_lstring(ctx, buf, len);
  return 1;
}

duk_ret_t duv_pipe_pending_instances(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"count", duk_is_number},
    {0,0}
  });
  uv_pipe_t *pipe = duv_require_this_handle(ctx, DUV_TCP_MASK);
  uv_pipe_pending_instances(pipe,
    duk_get_int(ctx, 1)
  );
  return 0;
}

duk_ret_t duv_pipe_pending_count(duk_context *ctx) {
  uv_pipe_t *pipe = duv_require_this_handle(ctx, DUV_TCP_MASK);
  duk_push_int(ctx, uv_pipe_pending_count(pipe));
  return 1;
}

duk_ret_t duv_pipe_pending_type(duk_context *ctx) {
  uv_pipe_t *pipe = duv_require_this_handle(ctx, DUV_TCP_MASK);
  uv_handle_type type = uv_pipe_pending_type(pipe);
  const char* type_name;
  switch (type) {
#define XX(uc, lc) \
    case UV_##uc: type_name = #lc; break;
  UV_HANDLE_TYPE_MAP(XX)
#undef XX
    default: return 0;
  }
  duk_push_string(ctx, type_name);
  return 1;
}

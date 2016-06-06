#include "stream.h"
#include "utils.h"
#include "callbacks.h"

static duk_bool_t duv_is_stream(duk_context *ctx, int index) {
  return duv_is_handle_of(ctx, index, DUV_STREAM_MASK);
}

duk_ret_t duv_shutdown(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_shutdown %p", stream);
}

static void duv_on_connection(uv_stream_t *server, int status) {
  duk_context *ctx = server->data;
  if (status) {
    duk_push_error_object(ctx, DUK_ERR_ERROR, "%s: %s", uv_err_name(status), uv_strerror(status));
  }
  else {
    duk_push_null(ctx);
  }
  duv_call_callback((uv_handle_t*)server, "\xffon-connection", 1);
}

duk_ret_t duv_listen(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"backlog", duk_is_number},
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duk_put_prop_string(ctx, -3, "\xffon-connection");
  duv_check(ctx, uv_listen(stream,
    duk_get_int(ctx, 1),
    duv_on_connection
  ));
  return 0;
}

duk_ret_t duv_accept(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"client", duv_is_stream},
    {0,0}
  });
  uv_stream_t *server = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  uv_stream_t *client = duv_get_handle(ctx, 1);
  duv_check(ctx, uv_accept(server, client));
  return 0;
}

duk_ret_t duv_read_start(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-read");
  duv_check(ctx, uv_read_start(stream, duv_on_alloc, duv_on_read));
  return 0;
}

duk_ret_t duv_read_stop(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_read_stop");
}

duk_ret_t duv_write(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_write");
}

duk_ret_t duv_is_readable(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_is_readable");
}

duk_ret_t duv_is_writable(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_is_writable");
}

duk_ret_t duv_stream_set_blocking(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_stream_set_blocking");
}

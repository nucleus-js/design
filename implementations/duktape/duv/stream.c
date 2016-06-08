#include "stream.h"

static duk_bool_t duv_is_stream(duk_context *ctx, int index) {
  return duv_is_handle_of(ctx, index, DUV_STREAM_MASK);
}

duk_ret_t duv_shutdown(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-shutdown");
  uv_shutdown_t *req = duk_push_fixed_buffer(ctx, sizeof(*req));
  duk_put_prop_string(ctx, 0, "\xffreq-shutdown");
  duv_check(ctx, uv_shutdown(req, stream, duv_on_shutdown));
  return 0;
  // TODO: expose req to javascript so it can be canceled?
}

duk_ret_t duv_listen(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"backlog", duk_is_number},
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-connection");
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
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duk_del_prop_string(ctx, 0, "\xffon-read");
  duv_check(ctx, uv_read_stop(stream));
  return 0;
}

duk_ret_t duv_write(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"data", dschema_is_data},
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-write");
  uv_write_t *req = duk_push_fixed_buffer(ctx, sizeof(*req));
  duk_put_prop_string(ctx, 0, "\xffreq-write");
  uv_buf_t buf;
  duv_get_data(ctx, 1, &buf);
  duv_check(ctx, uv_write(req, stream, &buf, 1, duv_on_write));
  req->data = stream;
  return 0;
  // TODO: expose req to javascript so it can be canceled?
  // TODO: handle case where multiple concurrent writes are in flight for a
  //       single stream.
}

duk_ret_t duv_is_readable(duk_context *ctx) {
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duk_push_boolean(ctx, uv_is_readable(stream));
  return 1;
}

duk_ret_t duv_is_writable(duk_context *ctx) {
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duk_push_boolean(ctx, uv_is_writable(stream));
  return 1;
}

duk_ret_t duv_stream_set_blocking(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"blocking", duk_is_boolean},
    {0,0}
  });
  uv_stream_t *stream = duv_require_this_handle(ctx, DUV_STREAM_MASK);
  duv_check(ctx, uv_stream_set_blocking(stream,
    duk_get_int(ctx, 1)
  ));
  return 0;
}

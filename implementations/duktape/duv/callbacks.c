#include "callbacks.h"
#include "utils.h"

void duv_on_close(uv_handle_t *handle) {
  duv_call_callback(handle, "\xffon-close", 0);
}

void duv_on_timeout(uv_timer_t *timer) {
  duv_call_callback((uv_handle_t*)timer, "\xffon-timeout", 0);
}

void duv_on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  (void)(handle);
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;
}

void duv_on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  duk_context *ctx = stream->data;

  if (nread >= 0) {
    char* out;
    duk_push_null(ctx);
    out = duk_push_fixed_buffer(ctx, nread);
    memcpy(out, buf->base, nread);
  }

  free(buf->base);
  if (nread == 0) return;

  if (nread == UV_EOF) {
    duk_push_null(ctx); // no error
    duk_push_undefined(ctx); // undefined value to signify EOF
  }
  if (nread < 0) {
    duv_push_status(ctx, nread);
    duk_push_undefined(ctx);
  }

  duv_call_callback((uv_handle_t*)stream, "\xffon-read", 2);
}

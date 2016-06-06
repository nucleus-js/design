#include "tcp.h"
#include "utils.h"

duk_ret_t duv_tcp(duk_context *ctx) {
  uv_tcp_t *tcp = duk_push_fixed_buffer(ctx, sizeof(uv_tcp_t));
  duv_check(ctx, uv_tcp_init(duv_loop(ctx), tcp));
  duv_setup_handle(ctx, (uv_handle_t*)tcp, DUV_TCP);
  return 1;
}

duk_ret_t duv_tcp_open(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_tcp_open");
}

duk_ret_t duv_tcp_nodelay(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_tcp_nodelay");
}

duk_ret_t duv_tcp_keepalive(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_tcp_keepalive");
}

duk_ret_t duv_tcp_simultaneous_accepts(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_tcp_simultaneous_accepts");
}

duk_ret_t duv_tcp_bind(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_tcp_bind");
}

duk_ret_t duv_tcp_getpeername(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_tcp_getpeername");
}

duk_ret_t duv_tcp_getsockname(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_tcp_getsockname");
}

duk_ret_t duv_tcp_connect(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement duv_tcp_connect");
}

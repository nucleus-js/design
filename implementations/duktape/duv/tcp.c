#include "tcp.h"
#include "utils.h"

duk_ret_t duv_tcp(duk_context *ctx) {
  uv_tcp_t *tcp = duk_push_fixed_buffer(ctx, sizeof(uv_tcp_t));
  duv_check(ctx, uv_tcp_init(duv_loop(ctx), tcp));
  duv_setup_handle(ctx, (uv_handle_t*)tcp, DUV_TCP);
  return 1;
}

duk_ret_t duv_tcp_open(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"fd", duk_is_number},
    {0,0}
  });
  uv_tcp_t *tcp = duv_require_this_handle(ctx, DUV_TCP_MASK);
  duv_check(ctx, uv_tcp_open(tcp,
    duk_get_int(ctx, 1)
  ));
  return 0;
}

duk_ret_t duv_tcp_nodelay(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"enable", duk_is_boolean},
    {0,0}
  });
  uv_tcp_t *tcp = duv_require_this_handle(ctx, DUV_TCP_MASK);
  duv_check(ctx, uv_tcp_nodelay(tcp,
    duk_get_int(ctx, 1)
  ));
  return 0;
}

duk_ret_t duv_tcp_keepalive(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"enable", duk_is_boolean},
    {"delay", duk_is_number},
    {0,0}
  });
  uv_tcp_t *tcp = duv_require_this_handle(ctx, DUV_TCP_MASK);
  duv_check(ctx, uv_tcp_keepalive(tcp,
    duk_get_int(ctx, 1),
    duk_get_int(ctx, 2)
  ));
  return 0;
}

duk_ret_t duv_tcp_simultaneous_accepts(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"enable", duk_is_boolean},
    {0,0}
  });
  uv_tcp_t *tcp = duv_require_this_handle(ctx, DUV_TCP_MASK);
  duv_check(ctx, uv_tcp_simultaneous_accepts(tcp,
    duk_get_int(ctx, 1)
  ));
  return 0;
}

duk_ret_t duv_tcp_bind(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"host", duk_is_string},
    {"port", duk_is_number},
    {0,0}
  });
  uv_tcp_t *tcp = duv_require_this_handle(ctx, DUV_TCP_MASK);
  const char *host = duk_get_string(ctx, 1);
  int port = duk_get_number(ctx, 2),
      flags = 0;
  struct sockaddr_storage addr;
  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
  }
  duv_check(ctx, uv_tcp_bind(tcp,
    (struct sockaddr*)&addr,
    flags
  ));
  return 0;
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

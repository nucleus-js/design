#include "tcp.h"

duk_ret_t duv_new_tcp(duk_context *ctx) {
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

const char* duv_protocol_to_string(int family) {
  #ifdef AF_UNIX
    if (family == AF_UNIX) return "UNIX";
  #endif
  #ifdef AF_INET
    if (family == AF_INET) return "INET";
  #endif
  #ifdef AF_INET6
    if (family == AF_INET6) return "INET6";
  #endif
  #ifdef AF_IPX
    if (family == AF_IPX) return "IPX";
  #endif
  #ifdef AF_NETLINK
    if (family == AF_NETLINK) return "NETLINK";
  #endif
  #ifdef AF_X25
    if (family == AF_X25) return "X25";
  #endif
  #ifdef AF_AX25
    if (family == AF_AX25) return "AX25";
  #endif
  #ifdef AF_ATMPVC
    if (family == AF_ATMPVC) return "ATMPVC";
  #endif
  #ifdef AF_APPLETALK
    if (family == AF_APPLETALK) return "APPLETALK";
  #endif
  #ifdef AF_PACKET
    if (family == AF_PACKET) return "PACKET";
  #endif
  return NULL;
}

static void duv_push_sockaddr(duk_context *ctx, struct sockaddr_storage* address, int addrlen) {
  char ip[INET6_ADDRSTRLEN];
  int port = 0;
  if (address->ss_family == AF_INET) {
    struct sockaddr_in* addrin = (struct sockaddr_in*)address;
    uv_inet_ntop(AF_INET, &(addrin->sin_addr), ip, addrlen);
    port = ntohs(addrin->sin_port);
  } else if (address->ss_family == AF_INET6) {
    struct sockaddr_in6* addrin6 = (struct sockaddr_in6*)address;
    uv_inet_ntop(AF_INET6, &(addrin6->sin6_addr), ip, addrlen);
    port = ntohs(addrin6->sin6_port);
  }

  duk_push_object(ctx);
  duk_push_string(ctx, duv_protocol_to_string(address->ss_family));
  duk_put_prop_string(ctx, -2, "family");
  duk_push_number(ctx, port);
  duk_put_prop_string(ctx, -2, "port");
  duk_push_string(ctx, ip);
  duk_put_prop_string(ctx, -2, "ip");
}

duk_ret_t duv_tcp_getpeername(duk_context *ctx) {
  uv_tcp_t *tcp = duv_require_this_handle(ctx, DUV_TCP_MASK);
  struct sockaddr_storage address;
  int addrlen = sizeof(address);
  duv_check(ctx, uv_tcp_getpeername(tcp, (struct sockaddr*)&address, &addrlen));
  duv_push_sockaddr(ctx, &address, addrlen);
  return 1;
}

duk_ret_t duv_tcp_getsockname(duk_context *ctx) {
  uv_tcp_t *tcp = duv_require_this_handle(ctx, DUV_TCP_MASK);
  struct sockaddr_storage address;
  int addrlen = sizeof(address);
  duv_check(ctx, uv_tcp_getsockname(tcp, (struct sockaddr*)&address, &addrlen));
  duv_push_sockaddr(ctx, &address, addrlen);
  return 1;
}

duk_ret_t duv_tcp_connect(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"host", duk_is_string},
    {"port", duk_is_number},
    {"callback", dschema_is_continuation},
    {0,0}
  });
  uv_tcp_t *tcp = duv_require_this_handle(ctx, DUV_TCP_MASK);
  duk_put_prop_string(ctx, 0, "\xffon-connect");
  uv_connect_t *req = duk_push_fixed_buffer(ctx, sizeof(*req));
  duk_put_prop_string(ctx, 0, "\xffreq-connect");
  const char *host = duk_get_string(ctx, 1);
  int port = duk_get_number(ctx, 2);
  struct sockaddr_storage addr;
  if (uv_ip4_addr(host, port, (struct sockaddr_in*)&addr) &&
      uv_ip6_addr(host, port, (struct sockaddr_in6*)&addr)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "Invalid IP address or port");
  }
  duv_check(ctx, uv_tcp_connect(req, tcp, (struct sockaddr*)&addr, duv_on_connect));
  return 0;
}

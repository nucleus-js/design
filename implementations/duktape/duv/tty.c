#include "tty.h"

duk_ret_t duv_new_tty(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"fd", duk_is_number},
    {"readable", duk_is_boolean},
    {0,0}
  });
  uv_tty_t *tty = duk_push_fixed_buffer(ctx, sizeof(uv_tty_t));
  duv_check(ctx, uv_tty_init(duv_loop(ctx), tty,
    duk_get_int(ctx, 0),
    duk_get_int(ctx, 1)
  ));
  duv_setup_handle(ctx, (uv_handle_t*)tty, DUV_TTY);
  return 1;
}

duk_ret_t duv_tty_set_mode(duk_context *ctx) {
  dschema_check(ctx, (const duv_schema_entry[]) {
    {"mode", duk_is_number},
    {0,0}
  });
  uv_tty_t *tty = duv_require_this_handle(ctx, DUV_TTY_MASK);
  duv_check(ctx, uv_tty_set_mode(tty,
    duk_get_int(ctx, 1)
  ));
  return 0;
}

duk_ret_t duv_tty_get_winsize(duk_context *ctx) {
  uv_tty_t *tty = duv_require_this_handle(ctx, DUV_TTY_MASK);
  int width, height;
  duv_check(ctx, uv_tty_get_winsize(tty, &width, &height));
  duk_push_array(ctx);
  duk_push_int(ctx, width);
  duk_put_prop_index(ctx, -2, 0);
  duk_push_int(ctx, height);
  duk_put_prop_index(ctx, -2, 1);
  return 1;
}

duk_ret_t duv_tty_reset_mode(duk_context *ctx) {
  duv_check(ctx, uv_tty_reset_mode());
  return 0;
}

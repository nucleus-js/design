#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "duktape-releases/src/duktape.h"
#define MINIZ_HEADER_FILE_ONLY
#include "../../deps/miniz.c"

static duk_ret_t nucleus_exit(duk_context *ctx) {
  exit(duk_require_int(ctx, 0));
}

int main(int argc, char *argv[]) {
  duk_context *ctx = duk_create_heap_default();
  if (!ctx) {
    printf("Failed to create a Duktape heap.\n");
    exit(1);
  }
  duk_push_object(ctx);

  duk_push_array(ctx);
  int argStart = 0;
  for (int i = 0; i < argc; i++) {
    if (argStart) {
      duk_push_string(ctx, argv[i]);
      duk_put_prop_index(ctx, -2, i - argStart);
    }
    else {
      if (strcmp(argv[i], "--") == 0) {
        argStart = i + 1;
      }
    }
  }
  duk_put_prop_string(ctx, -2, "args");

  duk_push_c_lightfunc(ctx, nucleus_exit, 1, 1, 0);
  duk_put_prop_string(ctx, -2, "exit");

  duk_push_object(ctx);
  #ifdef DUK_VERSION
  duk_push_string(ctx, "v");
  duk_push_int(ctx, DUK_VERSION / 10000);
  duk_push_string(ctx, ".");
  duk_push_int(ctx, (DUK_VERSION / 100) % 100);
  duk_push_string(ctx, ".");
  duk_push_int(ctx, DUK_VERSION % 100);
  duk_concat(ctx, 6);
  duk_put_prop_string(ctx, -2, "duktape");
  #endif
  #ifdef MZ_VERSION
  duk_push_string(ctx, "v");
  duk_push_string(ctx, MZ_VERSION);
  duk_put_prop_string(ctx, -2, "miniz");
  #endif

  duk_put_prop_string(ctx, -2, "versions");

  duk_push_string(ctx, "duktape");
  duk_put_prop_string(ctx, -2, "engine");

  duk_put_global_string(ctx, "nucleus");

  duk_eval_string(ctx, "print(JSON.stringify(nucleus));");
  duk_destroy_heap(ctx);
  return 0;
}

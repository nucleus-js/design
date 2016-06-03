#include <stdio.h>
#include "duktape-releases/src/duktape.h"
#define MINIZ_HEADER_FILE_ONLY
#include "../../deps/miniz.c"

int main(int argc, char *argv[]) {
  duk_context *ctx = duk_create_heap_default();
  if (!ctx) {
    printf("Failed to create a Duktape heap.\n");
    exit(1);
  }
  duk_eval_string(ctx, "print('Hello world!');");
  duk_destroy_heap(ctx);
  return 0;
}

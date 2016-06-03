#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>

#include "duktape-releases/src/duktape.h"
#define MINIZ_HEADER_FILE_ONLY
#include "../../deps/miniz.c"

static duk_ret_t nucleus_exit(duk_context *ctx) {
  exit(duk_require_int(ctx, 0));
  return 0;
}

static char* base;

// Removes count paths from stack and replaces with combined path
static void pathjoin(duk_context *ctx, const int count) {
  int i = count;
  int start = duk_get_top_index(ctx) - count + 1;
  int num = 0;
  while (i--) {
    if (duk_get_string(ctx, start + i)[0] == '/') break;
  }
  for (num = count -i; i < count; i++) {
    duk_dup(ctx, start + i);
    if (i < count - 1) duk_push_string(ctx, "/");
  }
  duk_concat(ctx, num * 2 - 1);
  char* final = canonicalize_file_name(duk_get_string(ctx, -1));
  if (final) {
    duk_pop_n(ctx, count + 1);
    duk_push_string(ctx, final);
  }
}

static void compile(duk_context *ctx, const char* code, const char* name) {
  duk_push_string(ctx, "function () {");
  duk_push_string(ctx, code);
  duk_push_string(ctx, "}");
  duk_concat(ctx, 3);
  duk_push_string(ctx, name);
  duk_compile(ctx, DUK_COMPILE_FUNCTION);
}

static duk_ret_t nucleus_compile(duk_context *ctx) {
  compile(ctx, duk_require_string(ctx, 0), duk_require_string(ctx, 1));
  return 1;
}

static void readfile(duk_context *ctx, const char* path) {
  FILE *file = fopen(path, "r");
  if (!file) {
    duk_push_null(ctx);
    return;
  }
  int count = 0;
  while (true) {
    char buf[1024];
    int num = fread(buf, 1, 1024, file);
    if (num <= 0) break;
    duk_push_lstring(ctx, buf, num);
    count++;
  }
  fclose(file);
  duk_concat(ctx, count);
}

static void resolve(duk_context *ctx, const char* path) {
  duk_push_string(ctx, base);
  duk_push_string(ctx, path);
  pathjoin(ctx, 2);
}


static duk_ret_t nucleus_readfile(duk_context *ctx) {
  resolve(ctx, duk_require_string(ctx, 0));
  readfile(ctx, duk_get_string(ctx, -1));
  return 1;
}

static int skipdots(const struct dirent *ent) {
  return strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..");
}

static duk_ret_t nucleus_scandir(duk_context *ctx) {
  resolve(ctx, duk_require_string(ctx, 0));
  const char* path = duk_get_string(ctx, -1);
  duk_require_function(ctx, 1);
  struct dirent **namelist;
  int n = scandir(path, &namelist, skipdots, alphasort);
  if (n < 0) {
    duk_push_false(ctx);
    return 1;
  }
  while (n--) {
    duk_dup(ctx, 1);
    duk_push_string(ctx, namelist[n]->d_name);
    switch (namelist[n]->d_type) {
      case DT_BLK: duk_push_string(ctx, "block"); break;
      case DT_CHR: duk_push_string(ctx, "character"); break;
      case DT_DIR: duk_push_string(ctx, "directory"); break;
      case DT_FIFO: duk_push_string(ctx, "fifo"); break;
      case DT_LNK: duk_push_string(ctx, "link"); break;
      case DT_REG: duk_push_string(ctx, "file"); break;
      case DT_SOCK: duk_push_string(ctx, "socket"); break;
      default: duk_push_string(ctx, "unknown"); break;
    }
    free(namelist[n]);
    duk_call(ctx, 2);
  }
  free(namelist);
  duk_push_true(ctx);
  return 1;
}

static duk_ret_t nucleus_dofile(duk_context *ctx) {
  resolve(ctx, duk_require_string(ctx, 0));
  const char* filename = duk_get_string(ctx, -1);
  readfile(ctx, filename);
  if (!duk_is_string(ctx, -1)) return 1;
  compile(ctx, duk_get_string(ctx, -1), filename);
  duk_call(ctx, 0);
  return 1;
}

static duk_ret_t nucleus_pathjoin(duk_context *ctx) {
  pathjoin(ctx, duk_get_top_index(ctx));
  return 1;
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
  for (int i = 1; i < argc; i++) {
    if (argStart) {
      duk_push_string(ctx, argv[i]);
      duk_put_prop_index(ctx, -2, i - argStart);
      continue;
    }
    if (strcmp(argv[i], "--") == 0) {
      argStart = i + 1;
      continue;
    }
    else if (argv[i][0] == '-') {
      printf("Unknown flag: %s\n", argv[i]);
      exit(1);
    }
    else if (base) {
      printf("base=%p\n", base);
      printf("Unexpected argument: %s\n", argv[i]);
      exit(1);
    }
    base = argv[i];
  }
  if (!base) {
    printf("Missing base path.");
    exit(1);
  }

  duk_put_prop_string(ctx, -2, "args");

  duk_push_string(ctx, "duktape");
  duk_put_prop_string(ctx, -2, "engine");

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

  char* cwd = get_current_dir_name();
  duk_push_string(ctx, cwd);
  free(cwd);
  duk_push_string(ctx, base);
  pathjoin(ctx, 2);
  base = (char*)duk_get_string(ctx, -1);
  duk_put_prop_string(ctx, -2, "base");

  duk_push_c_function(ctx, nucleus_exit, 1);
  duk_put_prop_string(ctx, -2, "exit");
  duk_push_c_function(ctx, nucleus_compile, 2);
  duk_put_prop_string(ctx, -2, "compile");
  duk_push_c_function(ctx, nucleus_readfile, 1);
  duk_put_prop_string(ctx, -2, "readfile");
  duk_push_c_function(ctx, nucleus_scandir, 2);
  duk_put_prop_string(ctx, -2, "scandir");
  duk_push_c_function(ctx, nucleus_dofile, 1);
  duk_put_prop_string(ctx, -2, "dofile");
  duk_push_c_function(ctx, nucleus_pathjoin, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "pathjoin");

  duk_put_global_string(ctx, "nucleus");

  // duk_eval_string(ctx, "print('hello')");
  duk_eval_string(ctx, "try{nucleus.dofile('main.js')}catch(err){print(err);nucleus.exit(1)}");

  duk_destroy_heap(ctx);

  return 0;
}

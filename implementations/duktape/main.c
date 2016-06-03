#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "duktape-releases/src/duktape.h"
#define MINIZ_HEADER_FILE_ONLY
#include "../../deps/miniz.c"

static duk_ret_t nucleus_exit(duk_context *ctx) {
  exit(duk_require_int(ctx, 0));
  return 0;
}

static char* base;
static mz_zip_archive zip;

static struct {
  // (path -- data or null)
  duk_c_function read;
  // (path cb -- exists) and call cb(name, type) for each entry
  duk_c_function scan;
} resource;

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
  // char* final = realpath(duk_get_string(ctx, -1), 0);
  // if (final) {
  //   duk_pop_n(ctx, count + 1);
  //   duk_push_string(ctx, final);
  // }
}

static duk_ret_t read_from_zip(duk_context *ctx) {
  const char* filename = duk_require_string(ctx, 0);
  size_t size = 0;
  char* data = mz_zip_reader_extract_file_to_heap(&zip, filename, &size, 0);
  if (data) {
    duk_push_lstring(ctx, data, size);
    free(data);
  }
  else {
    duk_push_null(ctx);
  }
  return 1;
}
static duk_ret_t scan_from_zip(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement scan_from_zip");
  return 0;
}

static void resolve_in_place(duk_context *ctx, int index) {
  duk_require_string(ctx, index);
  duk_push_string(ctx, base);
  duk_dup(ctx, index < 0 ? index - 1 : index);
  pathjoin(ctx, 2);
  duk_replace(ctx, index < 0 ? index - 1 : index);
}

static duk_ret_t read_from_disk(duk_context *ctx) {
  resolve_in_place(ctx, 0);
  const char* path = duk_require_string(ctx, 0);
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    if (errno == ENOENT) {
      duk_push_null(ctx);
      return 1;
    }
    duk_error(ctx, DUK_ERR_ERROR, "Failed to open %s: %s", path, strerror(errno));
    return 0;
  }
  int count = 0;
  while (true) {
    char buf[1024];
    ssize_t num = read(fd, buf, 1024);
    if (num == 0) break;
    if (num < 0) {
      close(fd);
      duk_error(ctx, DUK_ERR_ERROR, "Failed to read %s: %s", path, strerror(errno));
      return 0;
    }
    duk_push_lstring(ctx, buf, num);
    count++;
  }
  close(fd);
  duk_concat(ctx, count);
  return 1;
}
static duk_ret_t scan_from_disk(duk_context *ctx) {
  resolve_in_place(ctx, 0);
  duk_error(ctx, DUK_ERR_UNIMPLEMENTED_ERROR, "TODO: Implement scan_from_disk");
  return 0;
  // const char* path = duk_get_string(ctx, 0);
  // duk_require_function(ctx, 1);
  // struct dirent **namelist;
  // int n = scandir(path, &namelist, skipdots, alphasort);
  // if (n < 0) {
  //   duk_push_false(ctx);
  //   return 1;
  // }
  // while (n--) {
  //   duk_dup(ctx, 1);
  //   duk_push_string(ctx, namelist[n]->d_name);
  //   switch (namelist[n]->d_type) {
  //     case DT_BLK: duk_push_string(ctx, "block"); break;
  //     case DT_CHR: duk_push_string(ctx, "character"); break;
  //     case DT_DIR: duk_push_string(ctx, "directory"); break;
  //     case DT_FIFO: duk_push_string(ctx, "fifo"); break;
  //     case DT_LNK: duk_push_string(ctx, "link"); break;
  //     case DT_REG: duk_push_string(ctx, "file"); break;
  //     case DT_SOCK: duk_push_string(ctx, "socket"); break;
  //     default: duk_push_string(ctx, "unknown"); break;
  //   }
  //   free(namelist[n]);
  //   duk_call(ctx, 2);
  // }
  // free(namelist);
  // duk_push_true(ctx);
  // return 1;
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

static duk_ret_t nucleus_readfile(duk_context *ctx) {
  return resource.read(ctx);
}

static duk_ret_t nucleus_scandir(duk_context *ctx) {
  return resource.scan(ctx);
}

static duk_ret_t nucleus_dofile(duk_context *ctx) {
  const char* filename = duk_require_string(ctx, 0);
  resource.read(ctx);
  if (!duk_is_string(ctx, -1)) {
    duk_error(ctx, DUK_ERR_ERROR, "No such file %s", filename);
    return 1;
  }
  compile(ctx, duk_get_string(ctx, -1), filename);
  duk_call(ctx, 0);
  return 1;
}

static duk_ret_t nucleus_pathjoin(duk_context *ctx) {
  pathjoin(ctx, duk_get_top_index(ctx));
  return 1;
}

static void duk_put_nucleus(duk_context *ctx, int argc, char *argv[]) {
  // nucleus
  duk_push_object(ctx);

  // nucleus.args
  duk_push_array(ctx);
  for (int i = 0; i < argc; i++) {
    duk_push_string(ctx, argv[i]);
    duk_put_prop_index(ctx, -2, i);
  }
  duk_put_prop_string(ctx, -2, "args");

  // nucleus.engine
  duk_push_string(ctx, "duktape");
  duk_put_prop_string(ctx, -2, "engine");

  // nucleus.versions
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

  // nucleus.base
  duk_push_string(ctx, base);
  duk_put_prop_string(ctx, -2, "base");

  // nucleus.exit
  duk_push_c_function(ctx, nucleus_exit, 1);
  duk_put_prop_string(ctx, -2, "exit");
  // nucleus.compile
  duk_push_c_function(ctx, nucleus_compile, 2);
  duk_put_prop_string(ctx, -2, "compile");
  // nucleus.readfile
  duk_push_c_function(ctx, nucleus_readfile, 1);
  duk_put_prop_string(ctx, -2, "readfile");
  // nucleus.scandir
  duk_push_c_function(ctx, nucleus_scandir, 2);
  duk_put_prop_string(ctx, -2, "scandir");
  // nucleus.dofile
  duk_push_c_function(ctx, nucleus_dofile, 1);
  duk_put_prop_string(ctx, -2, "dofile");
  // nucleus.pathjoin
  duk_push_c_function(ctx, nucleus_pathjoin, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "pathjoin");

  duk_put_global_string(ctx, "nucleus");
}

int main(int argc, char *argv[]) {
  bool isZip = false;
  // If we detect a zip file appended to self, use it.
  if (mz_zip_reader_init_file(&zip, argv[0], 0)) {
    base = argv[0];
    isZip = true;
  } else {
    int i;
    for (i = 1; i < argc; i++) {
      if (strcmp(argv[i], "--") == 0) {
        i++;
        if (!base && i < argc) {
          base = argv[i++];
        }
        break;
      }
      if (argv[i][0] == '-') {
        fprintf(stderr, "Unknown flag: %s\n", argv[i]);
        exit(1);
      }
      if (base) {
        fprintf(stderr, "Unexpected argument: %s\n", argv[i]);
        exit(1);
      }
      base = argv[i];
    }
    if (!base) {
      fprintf(stderr, "Missing path to app and no embedded zip detected\n");
      exit(1);
    }
    argc -= i;
    argv = &(argv[i]);

    if (mz_zip_reader_init_file(&zip, base, 0)) {
      isZip = true;
    }
  }

  base = realpath(base, 0);

  if (isZip) {
    resource.read = read_from_zip;
    resource.scan = scan_from_zip;
  }
  else {
    resource.read = read_from_disk;
    resource.scan = scan_from_disk;
  }

  // Setup context with global.nucleus
  duk_context *ctx = duk_create_heap_default();
  duk_put_nucleus(ctx, argc, argv);

  // Run main.js function
  printf("\n\nRunning %s:main.js", base);
  if (isZip) {
    printf(" from zip.\n");
  }
  else {
    printf(" from filesystem.\n");
  }
  duk_push_string(ctx, "nucleus.dofile('main.js')");
  if (duk_peval(ctx)) {
    fprintf(stderr, "Uncaught Error: %s\n", duk_safe_to_string(ctx, -1));
    exit(1);
  }

  duk_destroy_heap(ctx);

  return 0;
}

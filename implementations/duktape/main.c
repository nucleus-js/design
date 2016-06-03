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

typedef struct duv_list {
  const char* part;
  int offset;
  int length;
  struct duv_list* next;
} duv_list_t;

static duv_list_t* duv_list_node(const char* part, int start, int end, duv_list_t* next) {
  duv_list_t *node = malloc(sizeof(*node));
  node->part = part;
  node->offset = start;
  node->length = end - start;
  node->next = next;
  return node;
}

static duk_ret_t duv_path_join(duk_context *ctx) {
  duv_list_t *list = NULL;
  int absolute = 0;

  // Walk through all the args and split into a linked list
  // of segments
  {
    // Scan backwards looking for the the last absolute positioned path.
    int top = duk_get_top(ctx);
    int i = top - 1;
    while (i > 0) {
      const char* part = duk_require_string(ctx, i);
      if (part[0] == '/') break;
      i--;
    }
    for (; i < top; ++i) {
      const char* part = duk_require_string(ctx, i);
      int j;
      int start = 0;
      int length = strlen(part);
      if (part[0] == '/') {
        absolute = 1;
      }
      while (start < length && part[start] == 0x2f) { ++start; }
      for (j = start; j < length; ++j) {
        if (part[j] == 0x2f) {
          if (start < j) {
            list = duv_list_node(part, start, j, list);
            start = j;
            while (start < length && part[start] == 0x2f) { ++start; }
          }
        }
      }
      if (start < j) {
        list = duv_list_node(part, start, j, list);
      }
    }
  }

  // Run through the list in reverse evaluating "." and ".." segments.
  {
    int skip = 0;
    duv_list_t *prev = NULL;
    while (list) {
      duv_list_t *node = list;

      // Ignore segments with "."
      if (node->length == 1 &&
          node->part[node->offset] == 0x2e) {
        goto skip;
      }

      // Ignore segments with ".." and grow the skip count
      if (node->length == 2 &&
          node->part[node->offset] == 0x2e &&
          node->part[node->offset + 1] == 0x2e) {
        ++skip;
        goto skip;
      }

      // Consume the skip count
      if (skip > 0) {
        --skip;
        goto skip;
      }

      list = node->next;
      node->next = prev;
      prev = node;
      continue;

      skip:
        list = node->next;
        free(node);
    }
    list = prev;
  }

  // Merge the list into a single `/` delimited string.
  // Free the remaining list nodes.
  {
    int count = 0;
    if (absolute) {
      duk_push_string(ctx, "/");
      ++count;
    }
    while (list) {
      duv_list_t *node = list;
      duk_push_lstring(ctx, node->part + node->offset, node->length);
      ++count;
      if (node->next) {
        duk_push_string(ctx, "/");
        ++count;
      }
      list = node->next;
      free(node);
    }
    duk_concat(ctx, count);
  }
  return 1;
}

// Changes the first arg in place
static void canonicalize(duk_context *ctx) {
  duk_require_string(ctx, 0);
  duk_push_c_function(ctx, duv_path_join, DUK_VARARGS);
  duk_dup(ctx, 0);
  duk_call(ctx, 1);
  duk_replace(ctx, 0);
}

// Changes the first arg in place
static void resolve(duk_context *ctx) {
  duk_require_string(ctx, 0);
  duk_push_c_function(ctx, duv_path_join, DUK_VARARGS);
  duk_push_string(ctx, base);
  duk_dup(ctx, 0);
  duk_call(ctx, 2);
  duk_replace(ctx, 0);
}

static duk_ret_t read_from_zip(duk_context *ctx) {
  canonicalize(ctx);
  const char* filename = duk_get_string(ctx, 0);
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
  canonicalize(ctx);
  duk_require_function(ctx, 1);
  const char* input = duk_get_string(ctx, 0);
  int index = -1;
  if (*input) {
    duk_dup(ctx, 0);
    duk_push_string(ctx, "/");
    duk_concat(ctx, 2);
    input = duk_get_string(ctx, -1);
    duk_pop(ctx);
    index = mz_zip_reader_locate_file(&zip, input, 0, 0);
    if (index < 0) {
      duk_push_false(ctx);
      return 1;
    }
    if (!mz_zip_reader_is_file_a_directory(&zip, index)) {
      duk_error(ctx, DUK_ERR_ERROR, "%s is not a directory", input);
      return 0;
    }
  }
  int pathlen = strlen(input);
  char *path = malloc(pathlen + 1);
  memcpy(path, input, pathlen + 1);

  int num = mz_zip_reader_get_num_files(&zip);
  char entry[PATH_MAX];
  while (++index < num) {
    mz_uint size = mz_zip_reader_get_filename(&zip, index, entry, PATH_MAX);
    if (strncmp(path, entry, pathlen)) {
      break;
    }
    int offset = pathlen;
    if (entry[offset] == '/') {
      offset++;
    }
    size -= offset + 1;
    char* match = strchr(entry + offset, '/');
    if (match && match[1]) continue;
    duk_dup(ctx, 1);
    if (entry[offset + size - 1] == '/') {
      duk_push_lstring(ctx, entry + offset, size - 1);
      duk_push_string(ctx, "directory");
    }
    else {
      duk_push_lstring(ctx, entry + offset, size);
      duk_push_string(ctx, "file");
    }
    duk_call(ctx, 2);
    duk_pop(ctx);
  }
  free(path);
  duk_push_true(ctx);
  return 1;
}

static duk_ret_t read_from_disk(duk_context *ctx) {
  resolve(ctx);
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
  resolve(ctx);
  duk_require_function(ctx, 1);
  const char* path = duk_get_string(ctx, 0);
  struct dirent *dp;
  DIR *dir = opendir(path);
  if (!dir) {
    if (errno == ENOENT) {
      duk_push_null(ctx);
      return 1;
    }
    duk_error(ctx, DUK_ERR_ERROR, "Failed to opendir %s: %s", path, strerror(errno));
    return 0;
  }
  while ((dp = readdir(dir)) != NULL) {
    if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
      duk_dup(ctx, 1);
      duk_push_string(ctx, dp->d_name);
      switch (dp->d_type) {
        case DT_BLK: duk_push_string(ctx, "block"); break;
        case DT_CHR: duk_push_string(ctx, "character"); break;
        case DT_DIR: duk_push_string(ctx, "directory"); break;
        case DT_FIFO: duk_push_string(ctx, "fifo"); break;
        case DT_LNK: duk_push_string(ctx, "link"); break;
        case DT_REG: duk_push_string(ctx, "file"); break;
        case DT_SOCK: duk_push_string(ctx, "socket"); break;
        default: duk_push_string(ctx, "unknown"); break;
      }
      duk_call(ctx, 2);
      duk_pop(ctx);
    }
  }
  closedir(dir);
  duk_push_true(ctx);
  return 1;
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

static void duk_put_nucleus(duk_context *ctx, int argc, char *argv[], int argstart) {
  // nucleus
  duk_push_object(ctx);

  // nucleus.base
  duk_push_string(ctx, base);
  duk_put_prop_string(ctx, -2, "base");

  // nucleus.cmd
  duk_push_string(ctx, argv[0]);
  duk_put_prop_string(ctx, -2, "cmd");

  // nucleus.args
  duk_push_array(ctx);
  for (int i = argstart; i < argc; i++) {
    duk_push_string(ctx, argv[i]);
    duk_put_prop_index(ctx, -2, i - argstart);
  }
  duk_put_prop_string(ctx, -2, "args");

  // nucleus.rawArgs
  duk_push_array(ctx);
  for (int i = 0; i < argc; i++) {
    duk_push_string(ctx, argv[i]);
    duk_put_prop_index(ctx, -2, i);
  }
  duk_put_prop_string(ctx, -2, "rawArgs");

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
  duk_push_c_function(ctx, duv_path_join, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "pathjoin");

  duk_put_global_string(ctx, "nucleus");
}

int main(int argc, char *argv[]) {
  bool isZip = false;
  int argstart = 1;
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
    argstart = i;

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
  duk_put_nucleus(ctx, argc, argv, argstart);

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

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
#include "rust-path.h"

#include "duktape-releases/src/duktape.h"
#define MINIZ_HEADER_FILE_ONLY
#include "../../deps/miniz.c"
#include "duv/duv.h"
#include "env.h"

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


static duk_ret_t duv_path_join(duk_context *ctx) {
  const char* result = duk_get_string(ctx, 0);
  for (int i = 1; i < duk_get_top(ctx); i++) {
    const char* str = duk_get_string(ctx, i);
    result = rust_join(result, str);
  }
  duk_push_string(ctx, result);
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

static const duk_function_list_entry nucleus_functions[] = {
  {"envkeys", env_keys, 1},
  {"getenv", env_get, 1},
  {"setenv", env_set, 2},
  {"unsetenv", env_unset, 1},
  {"exit", nucleus_exit, 1},
  {"compile", nucleus_compile, 2},
  {"readfile", nucleus_readfile, 1},
  {"scandir", nucleus_scandir, 2},
  {"dofile", nucleus_dofile, 1},
  {"pathjoin", duv_path_join, DUK_VARARGS},
  {0,0,0}
};

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

  duk_put_function_list(ctx, -1, nucleus_functions);

  // nucleus.uv
  duv_push_module(ctx);
  duk_put_prop_string(ctx, -2, "uv");

  duk_put_global_string(ctx, "nucleus");
}

void print_version() {
  fprintf(stderr, "Nucleus v0.0.0\n");
}

void print_usage(const char* progname) {
  print_version();
  fprintf(stderr, "Usage:\n"
         "\n"
         "  %s source -- args...              Run app from source tree\n"
         "  %s source/custom.js -- args...    Run custom main script\n"
         "  %s source [-l] [-o target]        Build app\n"
         "\n"
         "Options:\n"
         "\n"
         "  -v | --version          Print version and exit\n"
         "  -h | --help             Print help and exit\n"
         "  -l | --linked           Link runtime instead of embedding\n"
         "  -o | --output target    Generate output binary at this path\n"
         "",
         progname, progname, progname);
}

void add_zip_dir(const char* path, const char* prefix) {
  struct dirent *dp;
  DIR *dir = opendir(path);
  if (!dir) {
    printf("path=%s\n", path);
    perror("Problem reading directory");
    exit(1);
  }
  char fullPath[PATH_MAX * 2];
  char fullName[PATH_MAX * 2];
  while ((dp = readdir(dir)) != NULL) {
    if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
      snprintf(fullPath, PATH_MAX * 2, "%s/%s", prefix, dp->d_name);
      switch (dp->d_type) {
        case DT_DIR:
          snprintf(fullPath, PATH_MAX * 2, "%s/%s", path, dp->d_name);
          snprintf(fullName, PATH_MAX * 2, "%s%s/", prefix, dp->d_name);
          printf("Adding %s\n", fullName);
          mz_zip_writer_add_mem(&zip, fullName, 0, 0, 0);
          add_zip_dir(fullPath, fullName);
          break;
        case DT_REG: case DT_LNK:
          snprintf(fullPath, PATH_MAX * 2, "%s/%s", path, dp->d_name);
          snprintf(fullName, PATH_MAX * 2, "%s%s", prefix, dp->d_name);
          printf("Adding %s\n", fullName);
          mz_zip_writer_add_file(&zip, fullPath, fullName, fullPath, 0, 4095);
          break;
        default:
          printf("Skipping %s/%s\n", prefix, dp->d_name);
          break;
      }
    }
  }
}

void build_zip(const char* source, const char* target, int linked) {
  print_version();
  char nucleus[PATH_MAX*2];
  size_t nucleus_size = PATH_MAX*2;
  uv_exepath(nucleus, &nucleus_size);
  printf("Creating %s\n", target);
  int outfd = open(target, O_WRONLY | O_CREAT, 0777);
  if (outfd < 0) {
    perror("Cannot create target binary");
    exit(1);
  }
  if (linked) {
    printf("Inserting link to %.*s\n", (int)nucleus_size, nucleus);
    write(outfd, "#!", 2);
    write(outfd, nucleus, nucleus_size);
    write(outfd, " --\n", 1);
  }
  else {
    fprintf(stderr, "TODO: embed nucleus\n");
    exit(1);
  }
  close(outfd);
  mz_zip_writer_init_file(&zip, target, 1024);
  add_zip_dir(source, "");
  exit(1);
}

int main(int argc, char *argv[]) {
  uv_setup_args(argc, argv);
  bool isZip = false;
  int argstart = 1;
  const char* entry = 0;
  // If we detect a zip file appended to self, use it.
  if (mz_zip_reader_init_file(&zip, argv[0], 0)) {
    base = argv[0];
    isZip = true;
  } else {
    int i;
    int linked = 0;
    int outIndex = 0;
    for (i = 1; i < argc; i++) {
      if (strcmp(argv[i], "--") == 0) {
        if (linked || outIndex) {
          print_usage(argv[0]);
          fprintf(stderr, "\nCannot pass app args while building app binary.\n");
          exit(1);
        }
        i++;
        if (!base && i < argc) {
          base = argv[i++];
        }
        break;
      }
      if (argv[i][0] == '-') {
        if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
          print_version();
          exit(1);
        }
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
          print_usage(argv[0]);
          exit(1);
        }
        if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--linked")) {
          linked = 1;
          continue;
        }
        if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {
          outIndex = ++i;
          continue;
        }
        print_usage(argv[0]);
        fprintf(stderr, "\nUnknown flag: %s\n", argv[i]);
        exit(1);
      }
      if (base) {
        print_usage(argv[0]);
        fprintf(stderr, "\nUnexpected argument: %s\n", argv[i]);
        exit(1);
      }
      base = argv[i];
    }
    if (!base) {
      print_usage(argv[0]);
      fprintf(stderr, "\nMissing path to app and no embedded zip detected\n");
      exit(1);
    }
    if (linked && !outIndex) {
      print_usage(argv[0]);
      fprintf(stderr, "\nLinked option was specified, but not out path was given\n");
      exit(1);
    }
    if (outIndex) {
      if (!argv[outIndex]) {
        print_usage(argv[0]);
        fprintf(stderr, "\nMissing target path for --output option\n");
        exit(1);
      }
      build_zip(base, argv[outIndex], linked);
      exit(0);
    }

    argstart = i;

    if (mz_zip_reader_init_file(&zip, base, 0)) {
      isZip = true;
    }
  }

  const char* originalBase = base;
  base = realpath(base, 0);
  if (!base) {
    print_usage(argv[0]);
    fprintf(stderr, "\nNo such file or directory: %s\n", originalBase);
    exit(1);
  }
  printf("base='%s'\n", base);
  const char* ext = rust_extension(base);
  if (ext && !strcmp(ext, "js")) {
    entry = rust_filename(base);
    base = (char*)rust_dirname(base);
  }
  printf("ext='%s' entry='%s' base='%s'\n", ext, entry, base);

  if (isZip) {
    resource.read = read_from_zip;
    resource.scan = scan_from_zip;
  }
  else {
    resource.read = read_from_disk;
    resource.scan = scan_from_disk;
  }

  // Setup context with global.nucleus
  uv_loop_t loop;
  uv_loop_init(&loop);
  duk_context *ctx = duk_create_heap(0, 0, 0, &loop, 0);

  duk_put_nucleus(ctx, argc, argv, argstart);

  // Run main.js function
  duk_push_string(ctx, "nucleus.dofile('");
  duk_push_string(ctx, entry ? entry : "main.js");
  duk_push_string(ctx, "')");
  duk_concat(ctx, 3);
  if (duk_peval(ctx)) {
    duk_dump_context_stderr(ctx);
    duk_get_prop_string(ctx, -1, "stack");
    fprintf(stderr, "Uncaught %s\n", duk_safe_to_string(ctx, -1));
    exit(1);
  }

  duk_destroy_heap(ctx);

  return 0;
}

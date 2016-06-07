#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "env.h"
#include <errno.h>
#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#elif !defined(_MSC_VER)
extern char **environ;
#endif

duk_ret_t env_keys(duk_context *ctx) {
  #ifndef _WIN32

    duk_push_array(ctx);
    for (unsigned int i = 0; environ[i]; ++i) {
      const char* var = environ[i];
      const char* s = strchr(var, '=');
      const size_t length = s ? (size_t)(s - var) : strlen(var);
      duk_push_lstring(ctx, var, length);
      duk_put_prop_index(ctx, -2, i);
    }

  #else // _WIN32

    int i = 0;
    int show_hidden = 0;
    WCHAR* p;
    WCHAR* environment = GetEnvironmentStringsW();
    if (!environment) return 0;
    p = environment;
    if (duk_is_boolean(ctx, 0)) {
      show_hidden = duk_get_boolean(ctx, 0);
    }

    duk_push_array(ctx);
    while (*p) {
      char* utf8;
      size_t utf8_len;
      WCHAR* s;

      if (*p == L'=') {
        // If the key starts with '=' it is a hidden environment variable.
        if (show_hidden) {
          s = wcschr(p + 1, L'=');
        }
        else {
          // Skip it
          p += wcslen(p) + 1;
          continue;
        }
      }
      else {
        s = wcschr(p, L'=');
      }

      if (!s) {
        s = p + wcslen(p);
      }
      // Convert from WCHAR to UTF-8 encoded char
      utf8_len = WideCharToMultiByte(CP_UTF8, 0, p, s - p, NULL, 0, NULL, NULL);
      utf8 = malloc(utf8_len);
      WideCharToMultiByte(CP_UTF8, 0, p, s - p, utf8, utf8_len, NULL, NULL);

      duk_push_lstring(ctx, utf8, utf8_len);
      duk_put_prop_index(ctx, -2, i++);

      free(utf8);

      p = s + wcslen(s) + 1;
    }
    FreeEnvironmentStringsW(environment);

  #endif

  return 1;
}

duk_ret_t env_get(duk_context *ctx) {
  const char* name = duk_require_string(ctx, 0);
  #ifdef _WIN32
    char* value;
    WCHAR* wname;
    WCHAR* wvalue;
    size_t wsize, size, wname_size;

    // Convert UTF8 char* name to WCHAR* wname with null terminator
    wname_size = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
    wname = malloc(wname_size * sizeof(WCHAR));
    if (!wname) return 0
    MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, wname_size);

    // Check for the key
    wsize = GetEnvironmentVariableW(wname, NULL, 0);
    if (!wsize) {
      free(wname);
      return 0;
    }

    // Read the value
    wvalue = malloc(wsize * sizeof(WCHAR));
    if (!wvalue) {
      free(wname);
      return 0;
    }
    GetEnvironmentVariableW(wname, wvalue, wsize);

    // Convert WCHAR* wvalue to UTF8 char* value
    size = WideCharToMultiByte(CP_UTF8, 0, wvalue, -1, NULL, 0, NULL, NULL);
    value = malloc(size);
    if (!value) {
      free(wname);
      free(wvalue);
      return 0;
    }
    WideCharToMultiByte(CP_UTF8, 0, wvalue, -1, value, size, NULL, NULL);

    duk_push_lstring(ctx, value, size - 1);

    free(wname);
    free(wvalue);
    free(value);

  #else
    duk_push_string(ctx, getenv(name));
  #endif
  return 1;
}

duk_ret_t env_set(duk_context *ctx) {
  const char* name = duk_require_string(ctx, 0);
  const char* value = duk_require_string(ctx, 1);

#ifdef _WIN32
  WCHAR* wname;
  WCHAR* wvalue;
  size_t wname_size, wvalue_size;
  int r;
  // Convert UTF8 char* name to WCHAR* wname with null terminator
  wname_size = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
  wname = malloc(wname_size * sizeof(WCHAR));
  if (!wname) return luaL_error(L, "Problem allocating memory for environment variable.");
  MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, wname_size);

  // Convert UTF8 char* value to WCHAR* wvalue with null terminator
  wvalue_size = MultiByteToWideChar(CP_UTF8, 0, value, -1, NULL, 0);
  wvalue = malloc(wvalue_size * sizeof(WCHAR));
  if (!wvalue) {
    free(wname);
    return 0;
  }
  MultiByteToWideChar(CP_UTF8, 0, value, -1, wvalue, wvalue_size);

  r = SetEnvironmentVariableW(wname, wvalue);

  free(wname);
  free(wvalue);
  if (r == 0) return 0;
#else
  int r = setenv(name, value, 1);
  if (r) {
    if (r == EINVAL) {
      duk_error(ctx, DUK_ERR_ERROR, "EINVAL: Invalid name.");
    }
    return 0;
  }
#endif
  return 0;
}

duk_ret_t env_unset(duk_context *ctx) {
  const char* name = duk_require_string(ctx, 0);

#ifdef __linux__
  if (unsetenv(name)) {
    if (errno == EINVAL) {
      duk_error(ctx, DUK_ERR_ERROR, "EINVAL: name contained an '=' character");
    }
    return 0;
  }
#elif defined(_WIN32)
  WCHAR* wname;
  size_t wname_size;
  // Convert UTF8 char* name to WCHAR* wname with null terminator
  wname_size = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
  wname = malloc(wname_size * sizeof(WCHAR));
  if (!wname) return 0;
  MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, wname_size);
  SetEnvironmentVariableW(wname, NULL);
#else
  unsetenv(name);
#endif
  return 0;
}

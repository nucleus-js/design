#ifndef UTILS_H
#define UTILS_H

#include "duv.h"
#include "dschema.h"

const char* duv_type_to_string(duv_type_t type);
const char* duv_mask_to_string(duv_type_mask_t mask);

// Retrieve the stored libuv loop from the duktape context.
uv_loop_t* duv_loop(duk_context *ctx);
// Push status as either error or null
void duv_push_status(duk_context *ctx, int status);
// Throw a libuv error in duktape.
void duv_error(duk_context *ctx, int status);
// Check return value from libuv function for errors.
void duv_check(duk_context *ctx, int status);

// Assumes buffer is at top of stack.
// replaces with this on top of stack.
void duv_setup_handle(duk_context *ctx, uv_handle_t *handle, duv_type_t type);

// Push the object belonging to a handle on the stack.
void duv_push_handle(duk_context *ctx, void *handle);
// Get the handle from object at index.
void* duv_get_handle(duk_context *ctx, int index);
// Schema helper for checking arguments to be a certain handle type
duk_bool_t duv_is_handle_of(duk_context *ctx, int index, duv_type_mask_t mask);
// Get the this handle.
void* duv_require_this_handle(duk_context *ctx, duv_type_mask_t mask);
// Assumes nargs are the top of the stack.  Rest comes from handle and key.
// Return value is not left on the stack.
void duv_call_callback(uv_handle_t* handle, const char* key, int nargs);

void duv_get_data(duk_context *ctx, int index, uv_buf_t *buf);

#endif

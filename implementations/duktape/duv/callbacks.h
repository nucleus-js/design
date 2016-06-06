#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "duv.h"

void duv_on_close(uv_handle_t *handle);
void duv_on_timeout(uv_timer_t *timer);
void duv_on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void duv_on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);

#endif

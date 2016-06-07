# LibUV API

This addon is located at `global.nucleus.uv` in the JS runtime.

## Event Loop

Libuv loop instances will not be exposed directly to JavaScript because it
generally complicates things needlessly.  If your VM supports having multiple
threads, then each one generally needs to have it's own event loop.  Pairing
event loops with VM contexts with threads seems to work well.

### `uv.run()`

This function blocks program execution and waits for events in a loop.
Generally this is the last line in your `main.js`.  In node.js, this is
implicitly run after running through the first tick of your first JS file.

### `uv.walk(callback(handle))`

This function iterates through all the handles currently in the loop.

## Handle

`Handle` is the base type for all libuv handle types.

All API methods defined here work with any handle type.The handle is the root
class of most the libuv structures.

### `handle.isActive() -> boolean`

Returns `true` if the handle is active, `false` if it’s inactive. What “active”
means depends on the type of handle:

- A `Async` handle is always active and cannot be deactivated, except by closing
  it with `async.close()`.
- A `Pipe`, `Tcp`, `Udp`, etc. handle - basically any handle that deals with
  i/o - is active when it is doing something that involves i/o, like reading,
  writing, connecting, accepting new connections, etc.
- A `Check`, `Idle`, `Timer`, etc. handle is active when it has been started
  with a call to `check.start()`, `idle.start()`, etc.

Rule of thumb: if a handle of type `uv_foo_t` has a `foo.start()` function, then
it’s active from the moment that function is called. Likewise, `foo.stop()`
deactivates the handle again.

### `handle.isClosing() -> boolean`

Returns true if the handle is closing or closed, false otherwise.

*Note: This function should only be used between the initialization of the
handle and the arrival of the close callback.*

### `handle.close(onClose)`

Request handle to be closed. `onClose` will be called asynchronously after this
call. This MUST be called on each handle before memory is released.

Handles that wrap file descriptors are closed immediately but `onClose` will
still be deferred to the next iteration of the event loop. It gives you a chance
to free up any resources associated with the handle.

In-progress requests, like `uv_connect_t` or `uv_write_t`, are canceled and have
their callbacks called asynchronously with `err=UV_ECANCELED`.

### `handle.ref()`

Reference the given handle. References are idempotent, that is, if a handle is
already referenced calling this function again will have no effect.

### `handle.unref()`

Un-reference the given handle. References are idempotent, that is, if a handle
is not referenced calling this function again will have no effect.

The libuv event loop (if run in the default mode) will run until there are no
active and referenced handles left. The user can force the loop to exit early by
unreferencing handles which are active, for example by calling `timer.unref()`
after calling `timer.start()`.

A handle can be referenced or unreferenced, the refcounting scheme doesn’t use a
counter, so both operations are idempotent.

All handles are referenced when active by default, see `handle.isActive` for a
more detailed explanation on what being active involves.

## Timer

Timer handles are used to schedule callbacks to be called in the future.

### `new Timer() -> timer`

Create a new timer instance.

### `timer.start(timeout, repeat, onTimeout)`

Start the timer. *timeout* and *repeat* are in milliseconds.

If *timeout* is zero, the callback fires on the next event loop iteration. If
*repeat* is non-zero, the callback fires first after timeout milliseconds and
then repeatedly after *repeat* milliseconds.

### `timer.stop()`

Stop the timer, the callback will not be called anymore.

### `timer.again()`

Stop the timer, and if it is repeating restart it using the repeat value as the
timeout. If the timer has never been started before it throws UV_EINVAL.

### `timer.setRepeat(repeat)`

Set the repeat interval value in milliseconds. The timer will be scheduled to
run on the given interval, regardless of the callback execution duration, and
will follow normal timer semantics in the case of a time-slice overrun.

For example, if a 50ms repeating timer first runs for 17ms, it will be scheduled
to run again 33ms later. If other tasks consume more than the 33ms following the
first timer callback, then the callback will run as soon as possible.

**Note: If the repeat value is set from a timer callback it does not immediately
take effect. If the timer was non-repeating before, it will have been stopped.
If it was repeating, then the old repeat value will have been used to schedule
the next timeout.**

### `timer.getRepeat() -> repeat`

Get the timer repeat value.

## `uv_prepare_t`

*TODO: document this module*

## `uv_check_t`

*TODO: document this module*

## `uv_idle_t`

*TODO: document this module*

## `uv_async_t`

*TODO: document this module*

## `uv_poll_t`

*TODO: document this module*

## `uv_signal_t`

*TODO: document this module*

## `uv_process_t`

*TODO: document this module*

## Stream

*TODO: document this module*

## Tcp

*TODO: document this module*

## `uv_pipe_t`

*TODO: document this module*

## `uv_tty_t`

*TODO: document this module*

## `uv_udp_t`

*TODO: document this module*

## `uv_fs_event_t`

*TODO: document this module*

## `uv_fs_poll_t`

*TODO: document this module*

## Filesystem

*TODO: document this module*

## DNS

*TODO: document this module*

## Miscellaneous

*TODO: document this module*

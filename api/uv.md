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

### `handle.isActive() → boolean`

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

### `handle.isClosing() → boolean`

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

---

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

### `new Timer() → timer`

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

### `timer.getRepeat() → repeat`

Get the timer repeat value.

## Prepare

Prepare handles will run the given callback once per loop iteration, right
before polling for i/o.

### `new uv.Prepare() → prepare`

Create a new Prepare instance.

### `prepare.start(onPrepare)`

Start the handle with the given callback.

### `prepare.stop()`

Stop the handle, the callback will no longer be called.

## Check

Check handles will run the given callback once per loop iteration, right after
polling for i/o.

### `new uv.Check() → check`

Create a new Check instance.

### `check.start(onCheck)`

Start the handle with the given callback.

### `check.stop()`

Stop the handle, the callback will no longer be called.

## Idle

Idle handles will run the given callback once per loop iteration, right before
the uv_prepare_t handles.

*Note: The notable difference with prepare handles is that when there are active
idle handles, the loop will perform a zero timeout poll instead of blocking for
i/o.*

**Warning: Despite the name, idle handles will get their callbacks called on
every loop iteration, not when the loop is actually “idle”.**

### `new uv.Idle() → idle`

Create a new Idle instance.

### `idle.start(onIdle)`

Start the handle with the given callback.

### `idle.stop()`

Stop the handle, the callback will no longer be called.

## Async

Async handles allow the user to “wakeup” the event loop and get a callback
called from another thread.

### `new uv.Async(onAsync)`

Create a new Async instance.

### `async.send()`

Wakeup the event loop and call the async handle’s callback.

*Note: It’s safe to call this function from any thread. The callback will be
called on the loop thread.

**Warning: libuv will coalesce calls to `async.send()``, that is, not every call
to it will yield an execution of the callback. For example: if `async.send()`
is called 5 times in a row before the callback is called, the callback will only
be called once. If `async.send()` is called again after the callback was called,
it will be called again.**

## `uv_poll_t`

*TODO: document this module*

## `uv_signal_t`

*TODO: document this module*

## `uv_process_t`

*TODO: document this module*

## Stream

Stream handles provide an abstraction of a duplex communication channel. Stream
is an abstract type, libuv provides 3 stream implementations in the form of Tcp,
Pipe, and Tty.

### `stream.shutdown(onShutdown) → shutdownReq`

Shutdown the outgoing (write) side of a duplex stream. It waits for pending
write requests to complete. The `onShutdown` callback is called after shutdown
is complete.

### `stream.listen(backlog, onConnection)`

Start listening for incoming connections. `backlog` indicates the number of
connections the kernel might queue, same as listen(2). When a new incoming
connection is received the `onConnection` callback is called.

### `stream.accept(socket)`

This call is used in conjunction with `stream.listen()` to accept incoming
connections. Call this function after receiving a `onConnection` call to accept
the connection.

When the `onConnection` callback is called it is guaranteed that this function
will complete successfully the first time. If you attempt to use it more than
once, it may fail. It is suggested to only call this function once per
`onConnection` call.

*Note: server and client must be handles running on the same loop.*

### `stream.readStart(onRead(err, data))`

Read data from an incoming stream. The `onRead` callback will be made several
times until there is no more data to read or `stream.readStop()` is called.

### `stream.readStop()`

Stop reading data from the stream. The `onRead` callback will no longer be
called.

This function is idempotent and may be safely called on a stopped stream.

### `stream.write(data, onWrite) → writeReq`

Write data to stream. `data` can be either a string or a buffer type.

### `stream.isReadable() → bool`

Returns true if the stream is readable, false otherwise.

### `stream.isWritable() → bool`

Returns true if the stream is writable, false otherwise.

## Tcp

TCP handles are used to represent both TCP streams and servers.

`Tcp.prototype` inherits from `Stream.prototype`.

### `tcp.open(fd)`

Open an existing file descriptor or SOCKET as a TCP handle.

The file descriptor is set to non-blocking mode.

### `tcp.nodelay(enable)`

Enable / disable Nagle’s algorithm.

### `tcp.simultaneous_accepts(enable)`

Enable / disable simultaneous asynchronous accept requests that are queued by
the operating system when listening for new TCP connections.

This setting is used to tune a TCP server for the desired performance. Having
simultaneous accepts can significantly improve the rate of accepting connections
(which is why it is enabled by default) but may lead to uneven load distribution
in multi-process setups.

### `tcp.bind(host, port)``

Bind the handle to an address and port. `host` is a string and can be an IPV4 or
IPV6 value.

When the port is already taken, you can expect to see an UV_EADDRINUSE error
from either `tcp.bind()`, `tcp.listen()` or `tcp.connect()`. That is, a
successful call to this function does not guarantee that the call to
`onConnection` or `onConnect` will succeed as well.

### `tcp.getsockname()` → {family, port, ip}

Get the current address to which the handle is bound.

### `tcp.getpeername()` → {family, port, ip}

Get the address of the peer connected to the handle.]

### `tcp.connect(host, port, onConnection)`

Establish an IPv4 or IPv6 TCP connection.

The callback is made when the connection has been established or when a
connection error happened.

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

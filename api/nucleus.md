# `global.nucleus` Interface

## System

These functions integrate with the operating system.

### nucleus.cmd

This is extracted from `argv[0]` in C's main function.

### nucleus.args

This will be the command-line args that were passed to the application.  It's
basically everything after `--` (or all the args past `argv[0]` in bundled
mode).

### nucleus.rawArgs

This is the full raw `argv` from C without any processing.

### nucleus.exit(code)

This allows exiting the process with an optional exit code.  If this is never
called and the main script returns, then the process exits with 0.

### nucleus.getenv(name) -> value

Read an environment variable by name

### nucleus.setenv(name, value)

Set an environment variable by name.

### nucleus.scanenv(callback(name, value))

Iterate through environment variables.

## Runtime

These give access to the JavaScript runtime.

### nucleus.engine

This is a string describing the JS engine being used.  It can contain values
like `"duktape"`, `"v8"`, etc.

### nucleus.compile(string, path) -> function

Compile a string into a JavaScript function.  The source is wrapped in `function () { ... }`
and run in function mode.

TODO: should add access to other compile modes?  How portable can this be
between engines?

## Resources

These APIs give access to the resource bundle.

### nucleus.base

Base is the base path of the resource bundle.  If running from a directory it
will be the directory path, if it's a zip, it will be path to the zip.

### nucleus.versions

A map of versions of nucleus and it's components.

### nucleus.readfile(path) -> data

This will load a file as data.  Note that data will be one byte per character so
that it's binary safe, but ASCII data will be usable as a string.  If you wish
to decode something that's UTF-8 encoded as a unicode string, do the following
[trick](http://ecmanaut.blogspot.com/2006/07/encoding-decoding-utf8-in-javascript.html).

```js
var code = decodeURIComponent(escape(nucleus.readfile("myfile.js")));
```

If the path is not a file, data will be `false`.

### nucleus.scandir(path, onEntry(name, type)) -> exists

This iterates through a directory calling your callback with name and type for
each entry.  It will return with `true` if the path was a folder and `false` if
not.

The `type` parameter in the callback is one of `"file"`, or `"directory"`
usually.

### nucleus.dofile(path) -> value

This is basically a combination of `nucleus.readfile` and `nucleus.compile`
which then executes the resulting function returning the result.  It's generally
used for bootstrapping an environment or module system.

### nucleus.pathjoin(...parts) -> path

Basic path-join to be used for paths to `dofile`, `scandir`, and `readfile`.

## Addons

In addition to this tiny core, various C libraries can be baked optionally into
the core and exposed through sub-objects.

### nucleus.uv

This exposed bindings to libuv. Full documentation can be found at
[docs/uv.md](./uv.md).

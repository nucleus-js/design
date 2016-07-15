# Nucleus API

Nucleus itself is a spec for an interface.  
This documents that interface, and the tests in
[`test/parallel`](../test/parallel) verify the interface.

## System

These functions integrate with the operating system.

### nucleus.cmd

A string extracted from `argv[0]` in C's `main()` function.

### nucleus.args

An array of strings containing the command-line args that were passed to the
application.  It's basically everything after `--` (or all the args past
`argv[0]` in bundled mode).

### nucleus.rawArgs

An array of strings containing the the full raw `argv` from C without any
processing.

### nucleus.exit(code)

A function which exits the process. A numeric value is required.

If this is never called and the main script returns, then the process exits with
0.

### nucleus.getenv(name) -> value

Read an environment variable by a string `name`.

Returns a `string` if the variable exists, otherwise `null`.

### nucleus.setenv(name, value)

Set an environment variable by a string `name`.

### nucleus.unsetenv(name)

Unset an environment variable by a string `name`.

### nucleus.envkeys(showHidden) -> array of keys

Returns an array of all environment variables.

On windows there are also hidden variables which can be shown by passing `true`.

## Runtime

These give access to the JavaScript runtime.

### nucleus.engine

A string describing the JS engine. It can contain values like `"duktape"`,
`"v8"`, etc.

### nucleus.compile(string, path) -> function

Compiles the `string` into a JavaScript function that takes zero arguments and
returns the last expression.  This is run in global context
with a file context of `path`, used in generating error stacktraces.

## Resources

These APIs give access to the resource bundle.

### nucleus.base

A string containing the base path of the resource bundle.  If running from a
directory it will be the directory path, if it's a zip, it will be path to the
zip.

### nucleus.versions

An object map containing the versions of nucleus's components.

```js
{ duktape: "v1.5.0", ... }
```

### nucleus.readfile(path) -> data

Loads a file as string data.  Note that data will be one byte per character so
that it's binary safe, but ASCII data will be usable as a string.  If you wish
to decode something that's UTF-8 encoded as a unicode string, do the following
[trick](http://ecmanaut.blogspot.com/2006/07/encoding-decoding-utf8-in-javascript.html).

```js
var code = decodeURIComponent(escape(nucleus.readfile("myfile.js")));
```

If the path is not a file, data will be `null`.

### nucleus.scandir(path, onEntry(name, type)) -> exists

This function iterates through a directory specified by `path`, calling the
`onEntry` callback with the strings `name` and `type` for each entry.

The function will return with `true` if the path was a folder.
An error will be thrown if `path` is not a directory but exists.
If the `path` does not exist, the function will return `null`.

The `type` parameter in the callback is one of `"file"`, or `"directory"`

### nucleus.dofile(path) -> value

A function which allows the execution of an external JS file specified by a
string `path`, and returns the value returned by that file.

This is a composition of `nucleus.readfile()` and `nucleus.compile()`, which
then executes the resulting function returning the result.

Useful for bootstrapping an environment or a module system.

### nucleus.pathjoin(...parts) -> path

A function which joins an array of parts into a returned `path`.
Used for paths in `dofile`, `scandir`, and `readfile`.

## Addons

In addition to this tiny core, various C libraries can be baked optionally into
the core and exposed through sub-objects.

### nucleus.uv

This exposed bindings to libuv. Full documentation can be found at
[api/uv.md](./uv.md).

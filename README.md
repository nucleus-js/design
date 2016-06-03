
Nucleus is a specification for a minimal JS runtime that makes it fun to create
your own platforms.

## Command Line Interface

```sh
# Show version and build options
nucleus --version

# Run an app from a tree in the filesystem.
nucleus app-src -- args...
# Run an app from a zip bundle.
nucleus app.zip -- args...

# Manually build a standalone binary with nucleus embedded.
cat /usr/local/bin/nucleus app.zip > app
chmod +x app
./app args...

# Manually build a standalone binary that links to system nucleus.
echo "#!/usr/local/bin/nucleus" > prefix
cat prefix app.zip > app
chmod + x app
./app args...

# Build app with linked nucleus in shebang
nucleus app.zip --linked --output app
```

## Basic Application Skeleton

This is a sample application that's using a node.js style require system.

```
app
├── main.js
└── node_modules
    └── node-core
        ├── bootstrap.js
        .
        .
        .
```

The only thing special is there must be an `main.js` at the root of the tree.  
This file will be run in the JS runtime with a global `nucleus` injected.

```js
// main.js
// Bootstrap the node environment with the dofile builtin.
nucleus.dofile('node_modules/node-core/bootstrap.js')(function (require, module) {
  // node code goes here
});
```

## JS `nucleus` Interface

This global is the only thing custom injected into the JavaScript global scope.
It provides access to all the C bindings in nucleus as well as some utility
functions to work with the bundle resources and the JS runtime.

### nucleus.args

This will be the command-line args that were passed to the application

### nucleus.engine

The name of the JS engine being used.

### nucleus.versions

A map of versions of nucleus and it's components.

### nucleus.exit(code)

This allows exiting the process with an optional exit code.  If this is never
called and the main script returns, then the process exits with 0.

### nucleus.compile(string, path) -> function

Compile a string into a JavaScript function.

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

This is basically a combination of `nucleus.readfile` and `nucleus.compile` which
then executes the resulting function returning the result.  It's generally used
for bootstrapping an environment or module system.

### nucleus.pathjoin(...parts) -> path

Basic path-join to be used for paths to `dofile`, `scandir`, and `readfile`.

### nucleus.base

Base is the base path of the resource bundle.  If running from a directory it
will be the directory path, if it's a zip, it will be path to the zip.

### nucleus.uv

This will be bindings to libuv exposed to JavaScript.

### nucleus.miniz

Bindings to miniz that's used for the bundle logic.

### nucleus...

Other bindings...

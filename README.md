# Nucleus
> a minimal platform-agnostic JS runtime

Nucleus is a specification for a minimal JS runtime that makes it fun to create
your own platforms.

## Up and Running

To get up and running with Nucleus implementations, check out the [implementations]
directory. The [duktape `README`] is a great place to start.

[implementations]: tree/master/implementations
[duktape `README`]: blob/master/implementations/duktape/README.md

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

You can see full API docs for this in [api/nucleus.md](./api/nucleus.md).

This folder is a concrete implementation of the Nucleus interface using DukTape as the JavaScript engine.

It's tested regularly on Linux and OSX.

## Status

Currently this port is still im progress, but there is enough to already start
writing userland modules that use the `nucleus` global and parts of libuv.

## Building

I'm currently using a `Makefile` to script building.  The libuv submodule
requires `automake` and `libtool` installed (via apt-get, homebrew, etc...).

Also make sure to include all submodules when cloning nucleus.

```sh
# Clone recursivly to get all submodules
git clone --recursive https://github.com/creationix/nucleus.git
# Go to this folder
cd nucleus/implementations/duktape
# Build using 4 parallel cores
make -j4
# Optionally run all the tests.
make test
```

When you're done building the `nucleus` binary can be used to run and build your
own tiny JS scripts.

## Testing

Run `make -j4 test`. It will also attempt (re)build if you have not already.

Additionally, there are some manually verified tests. To run these, use
`make test-manual`.

## Optimizing for Size

By default the Makefile builds in debug mode with `-g`.  If you want a tiny
binary modify the Makefile to have `CC= cc -Os`.  If you've done a build before,
make sure to run `make distclean` before trying a new build.

If you want a small static binary on linux using musl, install musl-tools on
your ubuntu system and set `CC= musl-gcc -Os -static` and rebuild.

For maximum smallness (especially with the static build), `strip` your nucleus
binary before appending the app zip to it.

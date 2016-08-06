# Nucleus Command Line Interface

This is the spec for the minimum CLI options that nucleus implementations should expose.

Implementations may also expose platform, vm, and other special options not found in the other implementations in addition to these.

## nucleus [options] <file or dir> [--] ...

Execute nucleus with the provided options on the specified app bundle and provide it with arguments if specified.

The app bundle may be either a directory, a Zip file, or a JS file.

The following rules apply depending on what is provided:

##### Directory

The directory will be considered the app bundle and nucleus will attempt to execute the default `main.js` file.

##### Zip file

The contents of the zip file will be considered the app bundle and nucleus will attempt to execute the default main.js file within it.

##### JS file

The file's parent directory will be considered the app bundle and nucleus will attempt to execute the specified file.

If a file is provided in combination with the `-N`, `--no-bundle` option as specified below, app bundle file access restrictions will not apply.

### `-h`, `--help`

Prints CLI usage info including the spec options and executable name (or path), then exits with code `0`.

While all implementations must implement a help option, it is not required to look the same provided it lists all the basic options in this spec.

##### Example:

```
Usage: nucleus FILE [options]

Options:
    -h, --help          print this help menu
    -v, --version       print the Nucleus version
    -o, --output FILE   create a bundle with embedded nucleus at the specified file
    -l, --linked        link nucleus instead of embedding
    -z, --zip-only      create zip bundle without embedding
    -N, --no-bundle     do not execute as a bundle
```

### `-v`, `--version`

Prints the implementation and nucleus version, then exists with code `0`.

The output should follow this format:
```
<impl name> v<impl version> implementing Nucleus v<nucleus version>
```

`<impl name>` should be the official name of the impl, but could be anything.

Versions must be valid [SemVer](http://semver.org/) versions in the format of `major.minor.patch` with a pre-pended `v`.

##### example:
```
rustyduk v0.0.0 implementing Nucleus v0.0.0
```

### `-o`, `--output` `FILE`

Set an output filename for a zipped app bundle to be written to.

Nucleus will be embedded and the file will be given a permissions mode of `777` on applicable operating systems.

##### example:
```
nucleus ./my-app-folder -o my-app
```

### `-l`, `--linked`

Requires the `-o`, `--output` option.

Instead of embedding nucleus, insert a shebang link to it at the beginning of the zipped app bundle.

The link should look like `#!/path/to/nucleus --\n`.

##### example:
```
nucleus ./my-app-folder -l -o my-app-linked
```

### `-z`, `--zip-only`

Requires the `-o`, `--output` option.

Instead of embedding nucleus, only create a zipped app bundle.

##### example:
```
nucleus ./my-app-folder -z -o my-app-bundle
```

### `-N`, `--no-bundle`

Do not execute in bundled mode.

This option requires that an exact file be specified for nucleus execute.

Any app bundle file access limitations must not apply when executing in this mode.

Useful for testing.

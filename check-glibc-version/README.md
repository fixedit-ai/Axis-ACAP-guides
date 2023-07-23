# Application to check version of libc and glib

This is a simple app that prints a message to the standard output containing both the compile-time and the run-time `glibc` version and the `glib` version.

The version of both `libc` (more specifically `glibc`) and the version of `glib` differs both with the different SDKs and the different camera models / firmware versions. Since the libraries are only backwards compatible it is important that the compile-time version is equal to or higher than the run-time version, otherwise there might be incompatibilities in the ABI. The incompatibilities will in most cases cause an error when starting the application but might show up as much more confusing errors such as segfaults.

## Building
The application is easiest to build with FApp CLI:
```bash
fappcli-build build . --arch aarch64
```

You can also build it from the `Makefile`. To build the binary and run it, simply clone and run:
```
make run-bin
```
See the `Makefile` for options like camera IP, username, password, architecture, etc.

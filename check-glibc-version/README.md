[![Build and test the check-glibc-version ACAP](https://github.com/fixedit-ai/Axis-ACAP-guides/actions/workflows/build_check_glibc_version.yml/badge.svg)](https://github.com/fixedit-ai/Axis-ACAP-guides/actions/workflows/build_check_glibc_version.yml)

# Application to check version of libc and glib

This is a simple app that prints a message to the standard output containing both the compile-time and the run-time `glibc` version and the `glib` version.

The version of both `libc` (more specifically `glibc`) and the version of `glib` differs both with the different SDKs and the different camera models / firmware versions. Since the libraries are only backwards compatible it is important that the compile-time version is equal to or higher than the run-time version, otherwise there might be incompatibilities in the ABI. The incompatibilities will in most cases cause an error when starting the application but might show up as much more confusing errors such as segfaults.

Install the application in the camera, start it and show the logs by pressing the three dots and then "App log". You should see something like this:
```
2024-08-16T10:08:12.981+02:00 axis-b8a44f3b41fc [ INFO    ] check_glibc[2221112]: GLIB compile-time version:	2.62.6
2024-08-16T10:08:12.984+02:00 axis-b8a44f3b41fc [ INFO    ] check_glibc[2221112]: GLIB runtime version:     	2.74.6
2024-08-16T10:08:12.984+02:00 axis-b8a44f3b41fc [ INFO    ] check_glibc[2221112]: GNU libc compile-time version:	2.31
2024-08-16T10:08:12.984+02:00 axis-b8a44f3b41fc [ INFO    ] check_glibc[2221112]: GNU libc runtime version:     	2.39
```

## Building
You can download [prebuilt .eap files here](https://github.com/fixedit-ai/Axis-ACAP-guides/releases/tag/check-glibc-v0.0.1).

### Build with the Makefile
If you do not have access to the FixedIT CLI tool, use the Makefile:
```bash
make package
```

```bash
make clean
```

```bash
make package CAM_HOST=aarch64
```

```bash
make run-bin
```
See the `Makefile` for options like camera IP, username, password, architecture, etc. For more info about the Makefile and other options, take a look at our [Hello World example](../hello_world/).

### Build with the FixedIT CLI tool
The application is easiest to build with FixedIT CLI:
```bash
fixeditcli-build build . --arch aarch64
```

To select a different SDK, use e.g.:
```bash
fixeditcli-build build --sdk-name acap-sdk --sdk-version 3.3 --arch aarch64
```

For more information about how the build is performed, see the [fixedit CLI readme](https://github.com/fixedit-ai/fappcli-readme).


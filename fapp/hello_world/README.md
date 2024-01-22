# FApp Hello World application
This application shows a minimal example of an application that is using the FApp (FixedIT Application) standard library and being built with the FApp CLI tool. Any normal ACAP can be built with `fappcli`, but this example demonstrates the most basic features that you gain when using `fappcli`.

The application is using the signal handler from `libfapp`, this sets up the application to handle signals like `SIGINT`, `SIGFAULT` and many more such that the application can release any peripherals and make any clean-up necessary. The application runs a Glib event loop and registers a callback that will stop the event loop whenever a signal is received.

The application is using `libfapp` for logging which logs messages to both the standard output but also the system log. This allows retrival of log messages from the Axis camera web interface but also makes it easy to run the application manually during debugging or development such that the log messages are printed directly to the terminal. This is useful when using cameras with older firmware since they did not capture stdout.

## Build the application
The application uses a `Dockerfile` which specifies the build environment. The application is build with the FApp CLI tool, e.g.:
```bash
fappcli-build build .
```

You can also build the application, install it in an available camera and run it interactivily in the terminal with a single command:
```bash
fappcli-deploy run .
```
This command will look in your `~/.fapp` config file for cameras, find a camera that is currently available and check the CPU architecture of the camera. The application is then built for the cameras architecture, and installed in the camera using the VAPIX API. Once this is done, the application is run over ssh with the terminal's pty connected to the application.

The FApp CLI tool will automatically set build arguments for the build, such as:
* `FAPP_BIN_NAME`: The name of the application binary to build based on the manifest file
* `FAPP_APP_NAME`: The display name of the application based on the manifest file
* `FAPP_APP_VERSION`: The version of the application as specified in the `manifest.json` file
* `FAPP_CFLAGS` and `FAPP_LDFLAGS`: Other compilation flags needed for the build

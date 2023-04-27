# FApp cURL application
This application shows another example of how the `fappcli` tool and the precompiled FApp libraries can be used to make application development for the Axis cameras easier.

## Build and install
The application is built with `fappcli-build` tool. All the build options such as prebuilt libraries to pull when building are specified in the `fapp-manifest.json` file.

To build the application, simply run:
```bash
fappcli-build build .
```

The tool will fetch all the libraries and requirements needed and build the application. It will also extract the `.eap` file and the build log from the Docker container. The build log is found in the file `build.log`. If the build would fail due to e.g. invalid C code or errors in the `Dockerfile`, the build root will be extracted from the docker container and can be found in the `build_root` directory. This simplifies the troubleshooting process.

Select ACAP SDK 3 by running:
```bash
fappcli-build build . --sdk-name acap-sdk
```
or ACAP Native SDK 4 with:
```bash
fappcli-build build --sdk-name acap-native-sdk
```
The latest minor of the SDK will be used but you can also specify one specific with:
```bash
fappcli-build build --sdk-name acap-native-sdk --sdk-version 1.7
```
These options can also be specified in the `fapp-manifest.json` file to avoid repeating the same commands on every build.

To build and install the application, run:
```bash
fappcli-deploy install .
```
The tool will automatically select one of the cameras connected to your computer, find the architecture for that camera and build the application for that architecture. The tool will then use VAPIX to install the application in the camera. You can also run:
```bash
fappcli-deploy run .
```
which will not only build and install the application but also run it in the camera and pipe all the output to the terminal. To select a specific camera, add the option `--camera <CAM_NAME>` or to select any camera with a specific architecture, type: `--arch <ARCH>`.

## Using precompiled libraries
A library specified with the command line argument `--lib` or specified in the `fapp-manifest.json` file will be fetched as a docker image and the corresponding docker build-arg will be set. The format of the build-arg is `FAPP_IMAGE_<LIBRARY_NAME>`, e.g. specifying `--lib curl` will make the library available as `FAPP_IMAGE_CURL`. The deployment structure of all libraries are the same:
```
/
├── library
│   ├── include
│   │   └── <HEADER_FILES>
│   ├── lib
│   │   └── <SHARED_OBJECTS>
│   ├── license
│   │   └── <LICENCE_FILES>
│   ├── bin
│   │   └── <OPTIONAL_CLI_TOOLS>
```

To use the library code, copy headers and libraries to a directory search by the compiler:
```Docker
ARG FAPP_IMAGE_CURL
FROM ${FAPP_IMAGE_CURL} as library-curl

FROM ${FAPP_IMAGE_BUILDER_BASE} as builder
COPY --from=library-curl /library /opt/app
```

To correctly include all the libraries license information in your applications license page in the Axis UI, cat the content of all libraries license files to the end of your applications license file:
```docker
RUN cat license/* >> LICENSE
```

The `LICENSE` files and the `lib` directory will always be included in the `.eap` file. To include the `bin` folder with camera CLI tools, specify the argument `--extra-package-files bin` or add the same line to the `fapp-manifest.json` file.

## Logging and signal handling with libfapp
The application does also make use of `libfapp`. In difference from the other libraries this is distributed as source code. The `fappcli` tool will automatically download the source code if the `--lib libfapp` option is specified or if it is present in the `fapp-manifest.json` file.

The application is using the signal handler from `libfapp`, this sets up the application to handle signals like `SIGINT`, `SIGFAULT` and many more such that the application can release any peripherals and make any clean-up necessary. The application runs a Glib event loop and registers a callback that will stop the event loop whenever a signal is received.

The application is using `libfapp` for logging which logs messages to both the standard output but also the system log. This allows retrival of log messages from the Axis camera web interface but also makes it easy to run the application manually during debugging or development such that the log messages are printed directly to the terminal.

## Other build parameters
The FApp CLI tool will automatically set build arguments for the build, such as:
* `FAPP_BIN_NAME`: The name of the application binary to build based on the manifest file
* `FAPP_APP_NAME`: The display name of the application based on the manifest file
* `FAPP_CFLAGS` and `FAPP_LDFLAGS`: Other compilation flags needed for the build
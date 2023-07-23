# Hello world application

This hello world application is a good base to create other ACAPs from. We use this ACAP application as a starting point for many of our tutorials and trainings at [FixedIT.ai](https://fixedit.ai).

This application is mostly similar to the official Axis examples but we have added an extra `Makefile` that makes prototyping easier. Although this can be used for production ACAPs, we recommend using our FApp CLI tool which you can see an example of in the [fapp/hello_world example](../fapp/hello_world).

Another difference between this application and the Axis examples is that we compile the application in a `Docker run` command instead of in the `Docker build` command. The benefits with this is that we get better output from the build, we can make use of the dependency handling in `make` and if a build fails we have the partial build tree for debuging. The downside is that we do not get separation of the builds for different architectures, if you change the architecture you need to `make clean` before building again.

The SDK used in this app is the Native SDK release 1.8. This means that older firmware can't run the applications since LIBC is too old. Therefore we have set the `embeddedSdkVersion` to `2.18` in the `manifest.json` file. To use the application in cameras running older firmware, use an older SDK release and set the `embeddedSdkVersion` to `2.16`.

## Building
The `Makefile` contains targets to build, install and run the ACAP. Before using the `install` or `run` target, make sure that you have ssh enabled in the camera. You can test this with `ssh root@<camera-ip>` which should open a shell in the camera.

To build everything and install the application in the camera specified in the `CAM_HOST` variable, run:
```
make install
```

To rebuild only the binary and run it using ssh, run:
```
make run
```

Note that `run` does not reinstall the package with the dependencies, nor will changes to the `manifest.json` file take effect. The `run` target is only usable when only the C-code has been changed since it only copies the compiled binary file before running it. To make a fresh install, type `make install` followed by `make run`.

Also note that `make run` will always run the application as `root` regardless of the user specified in the `manifest.json` file since it is run over ssh. To run the application as the app user, start it from the Axis user interface after running `make install`.

See the `Makefile` for options like camera IP, username, password, architecture, etc. These can be specified like:
```bash
make install CAM_HOST=root@192.168.0.91 CAM_ARCH=aarch64
```
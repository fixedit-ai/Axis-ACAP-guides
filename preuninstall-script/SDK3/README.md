# Preuninstall script built with SDK 3 (not supported on all cameras)
This example shows how SDK 3 also supports the preuninstall script that was officially added to Native SDK 4. This does however depend on a new runtime in the camera which is not supported in older firmware tracks. This example sets the `embeddedSdkVersion` tag in the `app/manifest.json` file to the low value `2.0` which means that the application can be installed and run on firmware prior to 10.7 but the preuninstall script will never get executed in these cameras as it will in the cameras with newer firmware. The postinstall script works as expected since that was added a lot earlier and exists in all common firmwares avaialable today.

During installation of the application it will place a text file in the camera at the path `/usr/local/packages/test-app-log.txt` with the datetime of installation. When the application is uninstalled from the camera it will add a new datetime to the same text file with the time when the uninstallation was performed. This behavior was not easily achieved prior to the addition of the preinstallation script functionality. Use cases for the preuninstallation script includes clean-up of any persistent changes performed by the application such as e.g. added configurations for the Apache web server.

## Build and install
Build the application:
```bash
make build
```

Install the application on the camera:
```bash
make install-eap
```

You can specify the hostname of the camera as put in your `.ssh/config` file or the IP address:
```bash
make install-eap CAM_NAME=my_cam_hostname_or_ip
```

If the architecture is not armv7hf, then specify that:
```bash
make install-eap CAM_ARCH=aarch64
```

To make the arguments persistent in the current shell, export them as environment variables:
```bash
export CAM_NAME=my_cam_hostname_or_ip
export CAM_ARCH=aarch64
```

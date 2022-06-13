# Preuninstall script built with Native SDK 4
This example shows how a preuninstall script can be used to perform any action before the application is uninstalled, e.g. clean up changes done by the application to the camera system.

During installation of the application it will place a text file in the camera at the path `/usr/local/package/test-app-log.txt` with the datetime of installation. When the application is uninstalled from the camera it will add a new datetime to the same text file with the time when the uninstallation was performed. This behavior was not easily achieved prior to the addition of the preinstallation script functionality. Use cases for the preuninstallation script includes clean-up of any persistent changes performed by the application such as e.g. added configurations for the Apache web server.

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

# ACAP application example with parameter handler
In some ACAP SDKs there is included functionality for parameter handling. This simplifies the configuration of ACAP applications since there is no need for the application to have its own webpages for configuration. Instead the `axparameter` library is used. When the application is installed the parameters are created in the camera's global parameter handler acording to the specification in the `manifest.json` file. These parameters are then modifiable using the cameras web GUI in the "Application" tab, the VAPIX API or the `parhandclient` command in the camera using e.g. ssh.

![Screenshot](../.images/paramlib.png)

To build and install the application, run:
```bash
make build
make install-eap
```

The application can then be found in the camera web interface. When the application is started it prints a message with the read parameter to the system log:
```
2022-04-23T18:45:41.126+02:00 axis-b8a44f39f301 [ INFO    ] ParameterhandlerTest[14585]: Initializing application
2022-04-23T18:45:41.182+02:00 axis-b8a44f39f301 [ INFO    ] ParameterhandlerTest[14585]: Reporting to server: http://host.domain
2022-04-23T18:45:41.182+02:00 axis-b8a44f39f301 [ INFO    ] ParameterhandlerTest[14585]: Set up finished.
```

## SDK Compatibility
This application is using the `axparameter` library. It is a legacy library that was added a long time ago (prior to SDK 2) and then briefly removed with the introduction of Native SDK 4 to be later reintroduced.

- SDK 3: OK
- SDK 4.0 - 4.10: INCOMPATIBLE
- SDK 4.11 - 4.12: NOT OFFICIALLY SUPPORTED
- SDK 4.13+: OK

## Common issues and notes
The library does suffer from quite a few bugs and issues, e.g. the application and the parameters name must not contain any other characters than a-z, A-Z or 0-9, i.e. no dash or underscore, otherwise the parameter handler will fail to deal with the applications parameters.

This legacy parameter handler is often not what you want to use. Talk to us at FixedIT.ai for more info on better alternatives.

To use functionality such as callbacks you will need to have a GMainLoop running.
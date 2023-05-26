# FApp example applications
These example ACAPs make use of the FApp (FixedIT Application) ecosystem, including prebuilt libraries and command line tools that make building applications quicker and easier.

## fapp/hello_world
This is the most basic example of an Hello World application that is using the foundational `libfapp` library for logging and signal handling. The application is built using `fappcli-build` and `fappcli-deploy`.

## fapp/curl_example
This is a slightly more complex example that also makes use of the precompiled libraries cURL and openssl. The application is using a `fapp-manifest.json` file and a `fapp-manifest.debug.json` file to persist good default arguments to the `fappcli` commands for the two most common build patterns.

## fapp/sentry_example
Using our prebuild sentry client it is easy to log telemetry data from all your edge devices to a cloud service where you can build both dashboards and interactive drilldown pages.

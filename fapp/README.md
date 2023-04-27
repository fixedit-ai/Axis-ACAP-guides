# FApp example applications
These example ACAPs make use of the FApp (FixedIT Application) ecosystem, including prebuilt libraries and command line tools that make building applications quicker and easier.

## fapp/hello_world
This is the most basic example of an Hello World application that is using the foundational `libfapp` library for logging and signal handling. The application is built using `fappcli-build` and `fappcli-deploy`.

## fapp/curl_example
This is a slightly more complex example that also makes use of the precompiled libraries cURL and openssl. The application is also using a simple version of `fapp-manifest.json` to persist good default arguments to the `fappcli` commands.
# ACAP guides and examples
This repository serves as a collection of guides for ACAP development for Axis cameras by showing example code. Each explained problem and solution example has their own folder.

## Preuninstall script
This directory shows how the preuninstall script can be used to execute a script before the ACAP application is uninstalled from the camera.

## Parameter handler
This directory shows how the built in parameter library is used in SDK 3.

## Check glibc version
This is a simple application that prints the build-time and run-time versions of `libc` and `glib` used in the camera and in the SDK. This application is helpful for debugging and compatibility checking.

## fapp/*
This directory contains examples of how to build ACAP applications with the FApp (FixedIT Application) CLI tools and libraries. These tools makes it easier and faster to work with ACAP applications for multiple targets and multiple application scenarios. Contact us for more information on how you can improve your development environment.

## .github/workflows/*
Example of CI/CD jobs to build and verify the ACAP applications.
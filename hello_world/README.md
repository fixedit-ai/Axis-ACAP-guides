# Hello world application

This hello world application is a good base to create other ACAPs from. We use this ACAP application as a starting point for many of our tutorials and trainings at [FixedIT.ai](https://fixedit.ai).

This application is mostly similar to the official Axis examples with a few improvements:
- We have added an extra `Makefile` that makes prototyping (building, installing and running) on Linux easier.
- We have added PowerShell scripts that makes prototyping (building, installing and running) on Windows easier.
- We have added a `logmsg` function which logs to both the systemlog and `stdout` for easier debugging.
Although this template can be used for production ACAPs, we recommend using our FApp CLI tool which you can see examples of in the [fapp/hello_world example](../fapp/hello_world), [fapp/curl_example example](../fapp/curl_example) and [fapp/sentry_example example](../fapp/sentry_example) ACAPs.

Another difference between this application and the Axis examples is that we compile the application in a `Docker run` command instead of in the `Docker build` command. Some benefits with this is that we get better output from the build, we can make use of the dependency handling in `make` which can significantly speed up compilation of complex projects since only affected object files needs to be recompiled and if a build fails we have the partial build tree for debuging. The downside is that we do not get separation of the builds for different architectures, if you have built for one architecture and want to build for another, you MUST first run `make clean` on Linux or `.\windows_scripts\clean.ps1` on Windows.

The SDK used in this app is the Native SDK release 1.8. This means that older firmware can't run the applications since LIBC is too old in the cameras. Therefore we have set the `embeddedSdkVersion` to `2.18` in the `manifest.json` file. To use the application in cameras running older firmware, use an older SDK release and set the `embeddedSdkVersion` to `2.16`.

## Linux
These instructions explains how to set up the camera, build the application and install the application in Linux. For Windows, see next section. If you are using MacOS, the Linux instructions should apply if you have the correct tools installed and configured.

### Set up camera
The script makes use of SSH to access the camera. We recommend setting up passwordless SSH using you keyfile and configuring an easy to remember hostname in the `~/.ssh/config` file. You can also set the default username to use in the camera in the SSH config file.

The content of the `~/.ssh/config` file can e.g. be the following to setup a nice name (`my-cam`) for the camera with IP `192.168.0.195`:
```
Host my-cam
	HostName 192.168.0.195
	User root
```

To configure passwordless SSH, generate an SSH key if you don't already have one:
```bash
ssh-keygen
```
Then install the SSH key in the camera:
```bash
ssh-copy-id my-cam
```
where `my-cam` may be substituted for the camera's username and IP address, such as `root@192.168.0.195`.

You should also make sure that you have SSH enabled in the cameras plain config.

You can now test this with `ssh my-cam` which should open a shell in the camera.

### Building and installing
The `Makefile` contains targets to build, install and run the ACAP. Before using the `install` or `run` target, make sure that you have SSH setup as described above.

To build the .eap file, run the following. The .eap file will then be found in the `app` directory.
```bash
make package
```

If you have only changed the C-code since you last installed the application, then it is enough to rebuild the binary.
```bash
make build
```

To build everything and install the application in the camera (specified in the `CAM_HOST` environment variable, or default at IP `192.168.0.90`), run:
```bash
make install
```

If you have only changed the C-code since you last installed the application, then it is enough to rebuild and reinstall the binary. This is significantly faster then rebuilding and installing the .eap package. To rebuild and reinstall only the binary, run:
```bash
make install-bin
```

To rebuild the binary and run it directly using ssh, run:
```bash
make run
```

Note that `make run` does not reinstall the package with the dependencies, nor will changes to the `manifest.json` file take effect. This can also only be used when there is already an older version of the application already installed. To make a fresh install, type `make install` followed by `make run`. Also note that `make run` will always run the application as `root` regardless of the user specified in the `manifest.json` file since it is run over SSH. To run the application as the app user, start it from the Axis user interface after running `make install`. When using `make run`, the application should first be stopped from the camera's user interface.

See the `Makefile` for options like camera IP, username, architecture, etc. These can either be exported as environment variables or specified in the command like this:
```bash
make install CAM_HOST=root@192.168.0.91 CAM_ARCH=aarch64
```
or:
```bash
make install CAM_HOST=my-cam CAM_ARCH=armv7hf
```

## Windows
These instructions explains how to set up the camera, build the application and install the application in Windows. We assume that you have basic tools for ACAP development installed, such as Docker desktop. The instructions refer to commands run in PowerShell and assumes you have enabled script execution with e.g. `Set-ExecutionPolicy RemoteSigned`.

### Set up camera
The script makes use of SSH to access the camera. We recommend setting up passwordless SSH using you keyfile and configuring an easy to remember hostname in the `$env:USERPROFILE\.ssh\config` file. You can also set the default username to use in the camera in the SSH config file.

The content of the `$env:USERPROFILE\.ssh\config` file can e.g. be the following to setup a nice name (`my-cam`) for the camera with IP `192.168.0.195`:
```
Host my-cam
	HostName 192.168.0.195
	User root
```

To configure passwordless SSH, generate an SSH key if you don't already have one:
```powershell
ssh-keygen.exe
```
Then install the SSH key in the camera:
```powershell
$publicKey = Get-Content -Path "$env:USERPROFILE\.ssh\id_rsa.pub"
ssh my-cam "echo $publicKey >> ~/.ssh/authorized_keys"
```
where `my-cam` may be substituted for the camera's username and IP address, such as `root@192.168.0.195`.

You should also make sure that you have SSH enabled in the cameras plain config.

You can now test this with `ssh my-cam` which should open a shell in the camera.

### Building and installing
Scripts for working with the ACAP on windows are located in the `windows_scripts` directory. Before using the `install-package.ps1` script, make sure that you have SSH setup as described above.

To build the .eap file, run the following in PowerShell. The .eap file will then be found in the `app` directory.
```powershell
.\windows_scripts\build-package.ps1
```

If you have only changed the C-code since you last installed the application, then it is enough to rebuild the binary.
```powershell
.\windows_scripts\build-package.ps1 -CompileOnly
```

To build everything and install the application in the camera (specified in the `-CamHost` argument, or default at IP `192.168.0.90`), run:
```powershell
.\windows_scripts\install-package.ps1
```

If you have only changed the C-code since you last installed the application, then it is enough to rebuild and reinstall the binary. This is significantly faster then rebuilding and installing the .eap package. To rebuild and reinstall only the binary, run:
```powershell
.\windows_scripts\install-package.ps1 -BinaryOnly
```

To rebuild the binary and run it directly using ssh, run:
```powershell
.\windows_scripts\install-package.ps1 -BinaryOnly -Run
```

Note that `-BinaryOnly` does not reinstall the package with the dependencies, nor will changes to the `manifest.json` file take effect. This can also only be used when there is already an older version of the application already installed. Also note that `-Run` will always run the application as `root` regardless of the user specified in the `manifest.json` file since it is run over SSH. To run the application as the app user, start it from the Axis user interface after running `.\windows_scripts\install-package.ps1`. When using `-Run`, the application should first be stopped from the camera's user interface.

See the powershell scripts for options like camera IP, username, architecture, etc. These can be specified like this:
```powershell
.\windows_scripts\install-package.ps1 -CamHost cam-a8 -CamArch aarch64
```
or:
```powershell
.\windows_scripts\install-package.ps1 -CamHost root@192.168.0.90 -CamArch armv7hf
```

# Where to find more
At FixedIT.ai we dedicate our time to help companies and developers become masters of ACAP application development for the Axis network cameras. We can help you create ACAPs or become more efficient while doing so yourself. For consultation, fill out the form on https://fixedit.ai or go to our e-learning portal at https://learning.fixedit.ai to increase your own skills.
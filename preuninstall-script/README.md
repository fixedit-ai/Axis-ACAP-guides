# Preuninstall script
This directory shows how the preuninstall script can be used to execute a script before the ACAP application is uninstalled from the camera. This can be used for clean-up purposes and is otherwise hard to achieve since the application does not know when it is going to be uninstalled. The preuninstall script solves this issue.

The preuninstall script is officially supported in Native SDK 4 which supports cameras with firmware 10.7 or newer. Here is however also an example showing that it is infact also usable in SDK 3, but it will not be executed if the cameras firmware is not new enough.

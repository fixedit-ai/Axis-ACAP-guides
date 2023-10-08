param(
    [switch]$BinaryOnly,
    [switch]$Run,
    [string]$CamHost = "root@192.168.0.90",
    [string]$CamArch = "armv7hf",
    [string]$FappCFlags = "",
    [string]$FappMakeOpts = ""
)

# Source the build-package.ps1 script to build the application,
# this will also get the common variables such as $BINARY_NAME.
if ($BinaryOnly) {
    . $PSScriptRoot\build-package.ps1 -CamArch $CamArch -FappCFlags $FappCFlags -FappMakeOpts $FappMakeOpts -CompileOnly
} else {
    . $PSScriptRoot\build-package.ps1 -CamArch $CamArch -FappCFlags $FappCFlags -FappMakeOpts $FappMakeOpts
}

# Either install the .eap file in the camera, or copy only
# the binary to the camera since that is much faster and works
# if only the C-code is changed.
if ($BinaryOnly) {
    echo "Copying binary only to camera $CamHost"
    
    # Copy the binary only, this is faster than installing the eap file but
    # can only be done when the eap file has been installed before.
    scp "$PSScriptRoot\..\app\$BINARY_NAME" "${CamHost}:/usr/local/packages/$BINARY_NAME"
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to copy application binary: Exit code $LASTEXITCODE"
    }
} else {
    echo "Installing package in camera $CamHost"

    # We will make use of a randomized name to avoid conflicting
    # with the VAPIX installer if the cleanup would fail.
    $TMP_NAME = -join ((65..90) + (97..122) | Get-Random -Count 10 | % { [char]$_ }) + ".eap"

    # Copy the local file to the remote host under the temporary name
    scp "$PSScriptRoot\..\app\$EAP_NAME" "${CamHost}:/tmp/$TMP_NAME"

    # Install the file on the remote host
    ssh $CamHost "acap-install /tmp/$TMP_NAME"
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to install .eap file: Exit code $LASTEXITCODE"
    }

    # Remove the temporary file from the remote host
    ssh $CamHost "rm /tmp/$TMP_NAME"
}

# Run the application on the camera if requested and show the
# output from the application in the shell.
if ($Run) {
    echo "Running application on camera $CamHost"
    ssh -t $CamHost "/usr/local/packages/$BINARY_NAME/$BINARY_NAME"
    if ($LASTEXITCODE -ne 0) {
        throw "Application exited with error code: Exit code $LASTEXITCODE"
    }
}
param(
    [switch]$CompileOnly,
    [string]$CamArch = "armv7hf",
    [string]$FappCFlags = "",
    [string]$FappMakeOpts = ""
)

# Source the config file where we read the common variables such as $BINARY_NAME.
. $PSScriptRoot\config.ps1 -CamArch $CamArch

# Build the builder Docker container
docker build $PSScriptRoot\.. --build-arg CAM_ARCH=$CamArch -t ${BUILDER_NAME}:${TAG}-$CamArch

# Package application inside Docker container
if ($CompileOnly) {
    echo "Compiling only"
    # Instead of building the full .eap package, we are only compiling the
    # application binary. This makes for much faster prototyping when only
    # the C-code is changed.
    # Adding the -t flag makes sure that we get the color coding from gcc.
    docker run --rm -t `
        -e FAPP_CFLAGS=${FappCFlags} -e FAPP_MAKE_OPTS=${FappMakeOpts} `
        -e FAPP_BIN_NAME=${BINARY_NAME} `
        -v "$PSScriptRoot\..\app:/opt/app" `
        ${BUILDER_NAME}:${TAG}-$CamArch bash -c 'source $SDK_ENV && make $FAPP_MAKE_OPTS'
} else {
    echo "Compiling and packaging"
    docker run --rm -t `
        -e FAPP_CFLAGS=$FappCFlags `
        -e FAPP_BIN_NAME=$BINARY_NAME `
        -v "$PSScriptRoot\..\app:/opt/app" `
        ${BUILDER_NAME}:${TAG}-$CamArch bash -c 'source $SDK_ENV && acap-build .'
}
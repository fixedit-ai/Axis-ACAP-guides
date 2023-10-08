param(
    [string]$CamArch = "armv7hf"
)

# Extract values from manifest.json
$manifest = Get-Content -Path $PSScriptRoot\..\app\manifest.json | ConvertFrom-Json
$APP_VERSION = $manifest.acapPackageConf.setup.version
$APP_FRIENDLY_NAME = $manifest.acapPackageConf.setup.friendlyName
$BINARY_NAME = $manifest.acapPackageConf.setup.appName

# To make sure we use the correct .eap file, we need to construct the name
# of the package file in the same way as the ACAP builder script does. This
# ensures that we use the correct package if we change the content of the
# manifest.json file before building the package.
$APP_FILE_VERSION = $APP_VERSION -replace "\.", "_"
$APP_FILE_NAME = $APP_FRIENDLY_NAME -replace " ", "_"
$EAP_NAME = "${APP_FILE_NAME}_${APP_FILE_VERSION}_${CamArch}.eap"

# Create a name and tag for the builder container
# We make sure that the tag is unique so that twoi scripts run concurrently
# will not accidentally use the other scripts image.
$BUILDER_NAME = "$BINARY_NAME-builder"
$TIME = Get-Date -Format "yyyyMMddHHmmss"
$GIT_HASH = & git rev-parse --short HEAD
$GIT_STATE = if (-not (& git status --porcelain)) { "" } else { "-dirty" }
$GIT_STR = $GIT_HASH + $GIT_STATE
$TAG = if ($GIT_STR) { "$GIT_STR-$TIME" } else { $TIME }
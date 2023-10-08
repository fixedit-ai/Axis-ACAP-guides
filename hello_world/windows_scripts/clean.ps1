# Source the config file where we read the common variables such as $BINARY_NAME.
. $PSScriptRoot\config.ps1

# List of items to remove
$itemsToRemove = @(
    $BINARY_NAME,
    "*.o",
    "*.eap",
    "*.eap.old",
    "*_LICENSE.txt",
    "${BINARY_NAME}-build.env",
    "package.conf",
    "package.conf.orig",
    "param.conf"
)

# Remove each item
foreach ($item in $itemsToRemove) {
    Remove-Item -Path $PSScriptRoot\..\app\$item -ErrorAction SilentlyContinue
}
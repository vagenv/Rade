#/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")") && . $ROOTDIR/var.sh

$UE_BUILD_BAT $PROJECT_NAME"Editor" $BUILD_PLATFORM $BUILD_CONFIGURATION "$UPROJECT_PATH" -waitmutex -NoHotReload


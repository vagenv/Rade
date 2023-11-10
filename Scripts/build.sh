#/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")")
. $ROOTDIR/var.sh

$UE_BUILD_BAT $PROJECT"Editor" $BUILD_PLATFORM $BUILD_CONFIGURATION "$UPROJECT_PATH" -waitmutex -NoHotReload


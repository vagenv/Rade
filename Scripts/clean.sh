#/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")") && . $ROOTDIR/var.sh

rm -rf $PROJECT_DIR/Intermediate/
rm -rf $PROJECT_DIR/Binaries/

$UE_BUILD_BAT $PROJECT_NAME"Editor" $BUILD_PLATFORM $BUILD_CONFIGURATION "$UPROJECT_PATH" -waitmutex -Clean


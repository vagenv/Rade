#/bin/sh

. $(pwd)/var.sh

rm -rf $PROJECT_DIR/Intermediate/
rm -rf $PROJECT_DIR/Binaries/

$UE_BUILD_BAT $PROJECT"Editor" $BUILD_PLATFORM $BUILD_CONFIGURATION "$UPROJECT_PATH" -waitmutex -Clean


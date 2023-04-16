#/bin/sh

. $(pwd)/var.sh
$UE_BUILD_BAT $PROJECT"Editor" $BUILD_PLATFORM $BUILD_CONFIGURATION "$UPROJECT_PATH" -waitmutex -NoHotReload
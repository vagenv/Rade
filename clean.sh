#/bin/sh

. $(pwd)/var.sh

rm -rf $PROJECT_DIR/Intermediate/
rm -rf $PROJECT_DIR/Binaries/

$UE_BUILD_BAT $PROJECT"Editor" Linux Clean "$UPROJECT_PATH" -waitmutex -Clean
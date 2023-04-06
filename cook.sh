#/bin/sh

. $(pwd)/var.sh

$UE_EDITOR_CMD $PROJECT "$UPROJECT_PATH" -run=cook -targetplatform=$OS_TARGET


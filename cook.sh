#/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")")
. $ROOTDIR/var.sh

$UE_EDITOR_CMD $PROJECT "$UPROJECT_PATH" -run=cook -targetplatform=$OS_TARGET


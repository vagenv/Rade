#/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")")
. $ROOTDIR/var.sh

$UE_EDITOR_EXE "$UPROJECT_PATH"


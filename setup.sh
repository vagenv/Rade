#/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")")
. $ROOTDIR/var.sh

$UE_BUILD_TOOL -projectfiles -project=$UPROJECT_PATH -game -rocket -progress


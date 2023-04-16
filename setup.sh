#/bin/sh

. $(pwd)/var.sh

$UE_BUILD_TOOL -projectfiles -project=$UPROJECT_PATH -game -rocket -progress


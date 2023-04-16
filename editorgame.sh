#/bin/sh

. $(pwd)/var.sh

$UE_EDITOR_EXE "$UPROJECT_PATH"  -game -log -windowed -resx=1280 -rexy=720


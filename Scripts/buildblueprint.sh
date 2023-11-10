#/bin/sh

ROOTDIR=$(dirname "$(realpath "$0")")
. $ROOTDIR/var.sh

$UE_EDITOR_CMD $UPROJECT_PATH \
   -editortest \
   -Execcmds="Automation SetFilter Stress, Automation list, Automation RunTests Project.Blueprints.Compile Blueprints" \
   -testexit="Automation Test Queue Empty"

   
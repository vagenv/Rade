#/bin/sh

. $(pwd)/var.sh

$UE_EDITOR_CMD $UPROJECT_PATH \
   -editortest \
   -Execcmds="Automation SetFilter Stress, Automation list, Automation RunTests Project.Blueprints.Compile Blueprints" \
   -testexit="Automation Test Queue Empty"
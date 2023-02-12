#/bin/bash

ROOTDIR=$(pwd)
UE_DIR=/home/shituser/ssd/unreal/Linux/Linux_Unreal_Engine_5.1.0/

PROJECT=Rade
PROJECT_DIR=$ROOTDIR
UPROJECT_PATH=$PROJECT_DIR/$PROJECT.uproject

UE_EDITOR_EXE=$UE_DIR/Engine/Binaries/Linux/./UnrealEditor
UE_EDITOR_CMD=$UE_DIR/Engine/Binaries/Linux/./UnrealEditor-cmd
UE_BUILD_TOOL=$UE_DIR/Engine/Binaries/DotNET/AutomationTool/UnrealBuildTool
UE_BUILD_BAT=$UE_DIR/Engine/Build/BatchFiles/Linux/Build.sh


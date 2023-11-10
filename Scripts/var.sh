#/bin/bash

SCRIPTDIR=$(dirname "$(realpath "$0")")
ROOTDIR=$(dirname "$SCRIPTDIR")
UE_DIR="$(dirname "$ROOTDIR")/UE/LINUX/UE_5.3"


BUILD_PLATFORM=Linux
BUILD_CONFIGURATION=Development
OS_TARGET=Linux

PROJECT=Rade
PROJECT_DIR=$ROOTDIR
UPROJECT_PATH=$PROJECT_DIR/$PROJECT.uproject

UE_EDITOR_EXE=$UE_DIR/Engine/Binaries/Linux/./UnrealEditor
UE_EDITOR_CMD=$UE_DIR/Engine/Binaries/Linux/./UnrealEditor-Cmd
UE_BUILD_TOOL=$UE_DIR/Engine/Binaries/DotNET/AutomationTool/UnrealBuildTool
UE_BUILD_BAT=$UE_DIR/Engine/Build/BatchFiles/Linux/Build.sh
UE_BUILD_UAT=$UE_DIR/Engine/Build/BatchFiles/RunUAT.sh


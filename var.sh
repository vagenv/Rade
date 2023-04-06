#/bin/bash

resolve_relative_path() (
    # If the path is a directory, we just need to 'cd' into it and print the new path.
    if [ -d "$1" ]; then
        cd "$1" || return 1
        pwd
    # If the path points to anything else, like a file or FIFO
    elif [ -e "$1" ]; then
        # Strip '/file' from '/dir/file'
        # We only change the directory if the name doesn't match for the cases where
        # we were passed something like 'file' without './'
        if [ ! "${1%/*}" = "$1" ]; then
            cd "${1%/*}" || return 1
        fi
        # Strip all leading slashes upto the filename
        echo "$(pwd)/${1##*/}"
    else
        return 1 # Failure, neither file nor directory exists.
    fi
)

ROOTDIR=$(pwd)
UE_DIR=$(resolve_relative_path $(pwd)/../UE_LINUX)

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


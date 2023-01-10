@echo off

set ROOTDIR=%~dp0
set ROOTDIR=%ROOTDIR:~0,-1%
set UE_DIR=E:/Games/UE_5.1

set PROJECT=Rade
set PROJECT_DIR=%ROOTDIR%
set UPROJECT_PATH=%PROJECT_DIR%/%PROJECT%.uproject

set UE_EDITOR_EXE=%UE_DIR%/Engine/Binaries/Win64/UnrealEditor.exe
set UE_EDITOR_CMD=%UE_DIR%/Engine/Binaries/Win64/UnrealEditor-cmd.exe
set UE_BUILD_TOOL=%UE_DIR%/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe 
set UE_BUILD_BAT=%UE_DIR%/Engine/Build/BatchFiles/Build.bat



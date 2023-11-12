@echo off

set BACKUPDIR=%cd%

pushd %~dp0\..
set ROOTDIR=%cd%
pushd %~dp0\..\..
set UE_DIR=%cd%\UE\WINDOWS\UE_5.3
cd %BACKUPDIR%

set BUILD_PLATFORM=Win64
set BUILD_CONFIGURATION=Development
set OS_TARGET=Windows

set PROJECT_NAME=Rade
set PROJECT_DIR=%ROOTDIR%
set UPROJECT_PATH=%PROJECT_DIR%\%PROJECT_NAME%.uproject
set PACKAGE_DIR=%PROJECT_DIR%\Package\%OS_TARGET%

set UE_EDITOR_EXE=%UE_DIR%\Engine\Binaries\Win64\UnrealEditor.exe
set UE_EDITOR_CMD=%UE_DIR%\Engine\Binaries\Win64\UnrealEditor-cmd.exe
set UE_BUILD_TOOL=%UE_DIR%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe
set UE_BUILD_BAT=%UE_DIR%\Engine\Build\BatchFiles\Build.bat
set UE_BUILD_UAT=%UE_DIR%\Engine\Build\BatchFiles\RunUAT.bat


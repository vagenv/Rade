@echo off

call %~dp0\var.bat

call %UE_BUILD_BAT% %PROJECT%Editor Win64 Development "%UPROJECT_PATH%" -waitmutex -NoHotReload
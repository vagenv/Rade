@echo off

call %~dp0\var.bat

start "" %BUILD_BAT% %PROJECT% Win64 Development "%UPROJECT_PATH%" -waitmutex -NoHotReload
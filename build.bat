@echo off

call %~dp0\var.bat

call %UE_BUILD_BAT% %PROJECT%Editor %BUILD_PLATFORM% %BUILD_CONFIGURATION% "%UPROJECT_PATH%" -waitmutex -NoHotReload
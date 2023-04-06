@echo off

call %~dp0\var.bat

rd /s /q %PROJECT_DIR%\Intermediate\
rd /s /q %PROJECT_DIR%\Binaries\

call %UE_BUILD_BAT% %PROJECT%Editor %BUILD_PLATFORM% %BUILD_CONFIGURATION% "%UPROJECT_PATH%" -waitmutex -Clean


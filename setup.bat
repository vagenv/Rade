@echo off

call %~dp0\var.bat

call %UE_BUILD_TOOL% -projectfiles -project=%UPROJECT_PATH% -game -rocket -progress


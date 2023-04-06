@echo off

call %~dp0\var.bat

call %UE_EDITOR_EXE% "%UPROJECT_PATH%" -game -log -windowed -resx=1280 -rexy=720


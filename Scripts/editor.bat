@echo off

call %~dp0\var.bat

start "" %UE_EDITOR_EXE% "%UPROJECT_PATH%" %*


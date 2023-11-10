@echo off

call %~dp0\var.bat

call %UE_EDITOR_CMD% "%UPROJECT_PATH%" -run=cook -targetplatform=%OS_TARGET%


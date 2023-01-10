@echo off

call %~dp0\var.bat

start "" %UE_EDITOR_CMD% "%UPROJECT_PATH%" -run=cook -targetplatform=Windows
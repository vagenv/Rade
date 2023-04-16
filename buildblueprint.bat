@echo off

call %~dp0\var.bat

call %UE_EDITOR_CMD% %UPROJECT_PATH% ^
   -editortest ^
   -Execcmds="Automation SetFilter Stress, Automation list, Automation RunTests Project.Blueprints.Compile Blueprints" ^
   -testexit="Automation Test Queue Empty"
@echo off

call %~dp0\var.bat

set PACKAGE_DIR=%PROJECT_DIR%/Build/Windows/
call %UE_BUILD_UAT% -ScriptsForProject="%UPROJECT_PATH%" Turnkey -command=VerifySdk -platform=Win64 -UpdateIfNeeded -EditorIO -EditorIOPort=65065  -project="E:/unreal/Rade/Rade.uproject" BuildCookRun -nop4 -utf8output -nocompileeditor -skipbuildeditor -cook  -project="E:/unreal/Rade/Rade.uproject" -target=Rade  -unrealexe="E:\Games\UE_5.1\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" -platform=Win64 -installed -stage -archive -package -build -pak -iostore -compressed -prereqs -archivedirectory="%PACKAGE_DIR%" -CrashReporter -clientconfig=Development -nocompile -nocompileuat
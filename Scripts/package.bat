@echo off


goto :main

@REM Custom function to create shortcuts
:Create_Shortcut
    set ShortcutPath=%1
    set TargetPath=%2
    set Arguments=%3

   powershell.exe -Command "$WshShell = New-Object -ComObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%ShortcutPath%'); $Shortcut.TargetPath = '%TargetPath%'; $Shortcut.Arguments = '%Arguments%'; $Shortcut.Save()"

    goto :eof
	

:main

@REM Get variables
call %~dp0\var.bat

@REM Package
call "%UE_BUILD_UAT%"                    ^
   -ScriptsForProject="%UPROJECT_PATH%"  ^
   Turnkey                               ^
   -command=VerifySdk                    ^
   -platform=%BUILD_PLATFORM%            ^
   -UpdateIfNeeded                       ^
   -EditorIO                             ^
   -EditorIOPort=49968                   ^
   -project="%UPROJECT_PATH%"            ^
   BuildCookRun                          ^
   -nop4                                 ^
   -utf8output                           ^
   -nocompileeditor                      ^
   -skipbuildeditor                      ^
   -cook                                 ^
   -project="%UPROJECT_PATH%"            ^
   -target=%PROJECT_NAME%                ^
   -unrealexe="%UE_EDITOR_CMD%"          ^
   -platform=%BUILD_PLATFORM%            ^
   -installed                            ^
   -stage                                ^
   -archive                              ^
   -package                              ^
   -build                                ^
   -pak                                  ^
   -iostore                              ^
   -compressed                           ^
   -prereqs                              ^
   -archivedirectory="%PACKAGE_DIR%"     ^
   -CrashReporter                        ^
   -clientconfig="%BUILD_CONFIGURATION%" ^
   -nocompile                            ^
   -nocompileuat

@REM Create EXE shortcuts
set EXE_PATH=%PACKAGE_DIR%\%PROJECT_NAME%.exe
call :Create_Shortcut "%PACKAGE_DIR%\%PROJECT_NAME%_PSO.lnk" "%EXE_PATH%" "-logPSO -clearPSODriverCache"
call :Create_Shortcut "%PACKAGE_DIR%\%PROJECT_NAME%_TRACE.lnk" "%EXE_PATH%" "-statnamedevents -trace=cpu,gpu,frame,logbookmark,file,loadtime -StatCmds=\"startfile\""
call :Create_Shortcut "%PACKAGE_DIR%\%PROJECT_NAME%_MEMORY.lnk" "%EXE_PATH%" "-trace=default,memory -StatCmds=\"startfile\""


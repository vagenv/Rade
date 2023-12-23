@echo off

call %~dp0\var.bat

@REM TODO: Replace prefix with build id 
set PSO_PREFIX=psoprefix
set SHADER_FORMAT=SF_VULKAN_SM6


set USER_PSO_DIR=%PACKAGE_DIR%\%PROJECT_NAME%\Saved\CollectedPSOs
set PROJECT_PSO_DIR=%PROJECT_DIR%\Saved\Cooked\%OS_TARGET%\%PROJECT_NAME%\Metadata\PipelineCaches
set ALL_PSO_DIR=%PROJECT_DIR%\PSOCache

@REM Copy user PSO
xcopy /s /i %USER_PSO_DIR% %ALL_PSO_DIR%

@REM Copy Engine PSO config
xcopy /s /i %PROJECT_PSO_DIR% %ALL_PSO_DIR%

call %UE_EDITOR_CMD% "%UPROJECT_PATH%"  ^
   -run=ShaderPipelineCacheTools expand ^
    %ALL_PSO_DIR%\*.rec.upipelinecache  ^
    %ALL_PSO_DIR%\*.shk                 ^
    %ALL_PSO_DIR%\%PSO_PREFIX%%PROJECT%%SHADER_FORMAT%.spc


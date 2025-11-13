@echo off
REM ===============================
REM Build CCS projects (Windows)
REM ===============================

SETLOCAL ENABLEEXTENSIONS
SET "ERRORLEVEL=0"

REM --- Default config variables ---
set "TI_ROOT=D:\ti\ccs2031"
set "WORKSPACE=D:\Documents\ccs-ws"

REM --- Paths ---
set "CCS_CLI=%TI_ROOT%\ccs\eclipse\ccs-server-cli.bat"

REM --- Build Projects ---
echo Cleaning workspace: %WORKSPACE%
call "%CCS_CLI%" -noSplash -workspace "%WORKSPACE%" -application com.ti.ccs.apps.buildProject -ccs.workspace -ccs.clean
echo Cleaning workspace finished with error %ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

echo Building workspace: %WORKSPACE%
call "%CCS_CLI%" -noSplash -workspace "%WORKSPACE%" -application com.ti.ccs.apps.buildProject -ccs.workspace
echo Cleaning workspace finished with error %ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

echo Done.
ENDLOCAL

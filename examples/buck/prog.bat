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
set "UNIFLASH_CLI=%TI_ROOT%\ccs\ccs_base\scripting\examples\uniflash\cmdLine\uniflash.bat"

set "CPU1_CCXML=%WORKSPACE%\empty_driverlib_project_cpu1\targetConfigs\TMS320F28377D.ccxml"
set "CPU2_CCXML=%WORKSPACE%\empty_driverlib_project_cpu2\targetConfigs\TMS320F28377D.ccxml"

set "CPU1_OUT=%WORKSPACE%/empty_driverlib_project_cpu1/CPU1_FLASH/empty_driverlib_project_cpu1.out"
set "CPU2_OUT=%WORKSPACE%/empty_driverlib_project_cpu2/CPU2_FLASH/empty_driverlib_project_cpu2.out"

REM --- Flash CPUs ---
echo Flashing CPU1...
call "%UNIFLASH_CLI%" -ccxml "%CPU1_CCXML%" -core C28xx_CPU1 -program "%CPU1_OUT%"

echo Flashing CPU1 finished with error %ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

echo Flashing CPU2...
call "%UNIFLASH_CLI%" -ccxml "%CPU2_CCXML%" -core C28xx_CPU2 -program "%CPU2_OUT%"

echo Flashing CPU2 finished with error %ERRORLEVEL%
IF %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

echo Done.
ENDLOCAL

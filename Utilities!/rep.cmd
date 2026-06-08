@echo off

rem file:    rep.cmd - repeats the specified command, %1 times
rem exec:    [eg] rep 5 net user
rem author:  Ben Mullan (c) 2025

setlocal EnableExtensions EnableDelayedExpansion

rem %1 must be a positive integer
if "%~1"=="" goto :showUsage
set "COUNT=%~1"
2>nul set /a COUNT+=0
if errorlevel 1 goto :showUsage
if %COUNT% lss 1 goto :showUsage

set "ALL=%*"
for /f "tokens=1* delims= " %%A in ("%ALL%") do set "CMD=%%B"
if not defined CMD goto :showUsage

echo.
echo -----------------------------------------
echo repeating %COUNT%x:	%CMD%
echo -----------------------------------------
echo.

set /a FAILS=0
set "LASTRC=0"

for /l %%I in (1,1,%COUNT%) do (
  echo ^>^>^> run %%I/%COUNT%
  rem using `cmd /s /c` preserves complex quoting, ampersands, pipes, etc
  cmd /s /c "%CMD%"
  set "RC=!ERRORLEVEL!"
  echo ^>^>^> run exit-code: !RC!
  if not "!RC!"=="0" set /a FAILS+=1
  set "LASTRC=!RC!"
  echo.
)

echo -----------------------------------------
echo runs: %COUNT%   failed: %FAILS%   last exit-code: %LASTRC%
echo -----------------------------------------

endlocal & exit /b %LASTRC%

:showUsage
echo usage:
echo   rep ^<count^> ^<command and args...^>
echo examples:
echo   rep 5 echo hello
echo   rep 3 "C:\Program Files\nodejs\node.exe" script.js
echo   rep 10 npx tsx main.ts --someArg=value
exit /b 2
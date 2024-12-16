@echo off

rem File:	sec.cmd - show-exit-code (BATCH VERSION); executes the passthrough command, then prints the %ERRORLEVEL%
rem	Exec:	[eg] sec net user nonexist
rem Author:	Ben Mullan 2024

for /f %%a in ('echo prompt $E ^| cmd') do @set "esc=%%a"
call %*
echo %esc%[32m [sec] exit code = %errorlevel% %esc%[0m
title ^<%errorlevel%^> %*
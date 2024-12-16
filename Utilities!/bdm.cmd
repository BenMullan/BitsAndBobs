@echo off

rem File:	dm.cmd - doorman (BATCH VERSION); executes the passthrough command, printing the CLAs and exit-code
rem	Exec:	[eg] dm net user nonexist
rem Author:	Ben Mullan 2024

:define-ascii-escape-sequece-for-command-line-colours
	setLocal enableDelayedExpansion
	for /f %%a in ('echo prompt $E ^| cmd') do set "esc=%%a"

:print-executable-path
	for /f %%o in ('where %1') do set "exePath=%%o"
	echo %esc%[36mtarget: %exePath%%esc%[0m

:print-command-line-args
	set claIndex=-2
	for %%c in (%*) do (
		set /a claIndex+=1
		if !claIndex! neq -1 echo %esc%[34margs.!claIndex!: %%c%esc%[0m
	)
	echo.

:execute-passthrough-command
	call %*
	echo.

:print-exit-code
	echo %esc%[32mexit-code: %errorLevel%%esc%[0m
	title ^<%errorLevel%^> %*
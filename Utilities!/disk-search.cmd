@echo off
rem file:	disk-search.cmd - search entire disk for file
rem exec:	[eg] cmd.exe /c disk-search.cmd *.txt
rem exec:	[eg] disk-search.cmd windows\notepad.*xe
rem author:	Ben Mullan 2024

setlocal
set "driveLetter=%cd:~0,2%"

rem if search-pattern not specified as arg, prompt for it
if "%1"=="" (
	
	echo.
	echo disk-search: - crawl the entire disk for files matching a pattern
	echo ^(no pattern provided in args^)
	echo.
	echo specify a search pattern; eg *.txt
	set /p "searchPattern=search pattern: "
	echo.
	
	set "pauseAtEnd=true"
	
) else (
	set "searchPattern=%1"
)

title searching %driveLetter%\ for "%searchPattern%"
dir "%driveLetter%\%searchPattern%" /s /b
title %cd%\

if "%pauseAtEnd%"=="true" (
	echo.
	pause > nul | echo press any key to exit...
)

endlocal
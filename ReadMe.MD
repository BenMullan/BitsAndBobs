# Bits and Bobs!
(Misc. useful programming files and resources) 

### Quick links...
- [altgr-to-menu-key](https://raw.githubusercontent.com/BenMullan/BitsAndBobs/main/MiscSource/AltGR_To_MenuKey.REG)
- [exe-programs-july25](https://drive.google.com/file/d/1-KBx2_JY0a3kfpQUIpI56b4_R03wUAlo/view?usp=sharing)
- [xhtml-1920x1080-webcam-viewer](https://raw.githubusercontent.com/BenMullan/BitsAndBobs/main/MiscSource/simple-webcam-viewer-1920-1080.xhtml)
- [win7-server2008r2-update-for-windows-update](https://raw.githubusercontent.com/BenMullan/BitsAndBobs/main/MiscSource/InstallationEssentials/Win7Server2008R2_UpdateForWindowsUpdate_Windows6.1-KB3138612-x64.msu)

### MullNet Utilities (Binaries)
A collection of useful programming, maths, art, and file-system tools! See [MNUtils Index](https://github.com/BenMullan/BitsAndBobs/blob/main/Utilities!/_Index.txt).

<br/><img src="https://raw.githubusercontent.com/BenMullan/BitsAndBobs/main/Utilities!/_MNUtils.png" width="100%" /><br/>

| Program                	| Use                                                                                    	|
|------------------------	|----------------------------------------------------------------------------------------	|
| AutoEMail.exe          	| En-masse emailer (Multiple recipients, Multiple rounds)                                	|
| BaseConverter.exe      	| Place-value (positional notation) artwork experimentation program                      	|
| CLAOutputter.exe       	| [CLI] Echoes command-line arguments back, to check what they were                      	|
| CMDLaunchUtility.exe   	| Launches CMD.exe on [Win] + [C]                                                        	|
| ColourScreen.exe       	| [CLI/GUI] Fills the screen with the specified solid colour (for Projectors)            	|
| ConsoleInfo.exe        	| [CLI] Displays configuraiton infomation for the current Win32 Console                  	|
| Doorman.exe            	| [CLI] (v2!) Shows CLAs, stdout, stderr, and exit-codes for command-line processes      	|
| EmptyFolderDel.exe     	| Finds empty and/or RegEx-matching folders in the FileSystem                            	|
| ExcelRender.exe        	| Renders an image onto an Excel Spreadsheet!                                            	|
| GetNTPTime.exe         	| [CLI] E.g. GetNTPTime.exe /NTPServer:Time.Windows.com                                  	|
| GetText.exe            	| [CLI] Writes the text contents of CLA[0] to StdOut. Copes with BOMs, unlike "more".    	|
| ImageAdorn.exe         	| [CLI] Overlays one image on another; E.g. ImageAdorn BaseImage.png Superimposement.jpg 	|
| ImageAdornAll.exe      	| [CLI] Overlays one image onto all images in a folder at random positions. See /?.      	|
| Inlook.exe             	| Simple IMAP/SMTP EMail client. Micky-take on "Outlook".                                	|
| IP.exe                 	| [CLI] Prints the current IP Address to the console                                     	|
| MagnitudeIncrement.exe 	| [CLI] Takes in a place-value Magnitude and its Tokens, and computes one higher         	|
| MD5FileHasher.exe      	| [CLI] Computes MD5 hash E.g. MD5FileHasher MyFile.txt                                  	|
| MD5Hasher.exe          	| Computes the MD5 hash of an entered string                                             	|
| MicroHTTP.exe          	| [CLI] Useful HTTP Server; See MicroHTTP.exe /?                                         	|
| MNKeyShortcuts.exe     	| Enables several useful Keyboard Shortcuts: [Win] + C/N/I/S/Shift+N                     	|
| MouseGone.exe          	| ...Take a guess                                                                        	|
| MsgBoxGen.exe          	| MessageBox designer. Supports export to VBS.                                           	|
| PlayWav.exe            	| [CLI/AudioSession] Plays a WAV in the background, optionally repeating. See /?.        	|
| PolygonCircles.exe     	| Polygon Artwork creator; try an obscure side length, and scroll away...                	|
| RubiksCube.exe         	| Visualiser for a 6-face Rubik's Cube. Supports cube serialisation.                     	|
| RunHidden.exe          	| [CLI] Creates a process without a GUI. See [Details] tab in explorer Propities.        	|
| SaveScreenshot.exe     	| [CLI/GUI] Saves a screenshot, optionally repeating every few seconds. See /?.          	|
| SendKeys.exe           	| [CLI/GUI] Emulates keystrokes. Run: "SendKeys.exe /?" for usage.                       	|
| SendKeysGUI.exe        	| A comprehensive GUI version of SendKeys, supporting export to vbs                      	|
| ShowExitCode.exe       	| [CLI] Runs the shell command in the CLAs, reporting process info afterwards            	|
| Sleep.exe              	| Puts the computer into the ACPI sleep power state, if supported                        	|
| SQLMaster.exe          	| Tiny version of SQL Server Managment Studio!                                           	|
| SudokuMistry.exe       	| Sudoku Game (Written for a friend)                                                     	|
| WebcamView.exe         	| Simple Video Device previewer. Use AMCAP for more complex version.                     	|
| WebGet.exe             	| [CLI] Simple alternative to cURL. E.g. WebGet http://benm.eu5.org/ds                   	|
| WindowShots.exe        	| Saves screenshots of all open Windows into a folder in the current directory           	|
| Win32\Beep32           	| Calls Win32 MessageBeep() function                                                     	|
| Win32\MaxVol           	| Instantly sets the system volume to 100%                                               	|
| Win32\MouseGone32      	| Slightly less effective version of the .NET MouseGone.exe (above)                      	|
| Win32\MouseWindowCap32 	| Saves a screenshot of the Window the mouse-pointer is currently over.                  	|
| bdm.cmd                	| [CLI] Batch-file version of Doorman.exe; doesn't impeed non-linebreak-terminated stdout	|
| sec.cmd                	| [CLI] Batch-file version of ShowExitCode.exe; doesn't impeed non-linebreak-terminated...	|
| disk-search.cmd        	| [CLI] Searches entire hard-disk for a file pattern eg "*.txt"                          	|

E.g. Use `RunHidden.exe` to run SaveScreenshot or PlayWav as invisible processes via PsExec on a remote computer.
Parts of the source for these binaries is in [BenMullan/MullNetUtilities](https://github.com/BenMullan/MullNetUtilities)

_Ben Mullan (c) 2025_
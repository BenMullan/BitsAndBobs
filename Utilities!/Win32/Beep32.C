/*
	Beep32.C - Plays a beep through the Soundcard, or, failing that, the PC (BIOS) Speaker.
	Ben Mullan 2022
	-
	Compile with: cl.exe Beep32.CPP
*/

#include <Windows.H>
#pragma comment(lib, "User32.Lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd) {
	
	MessageBeep(0xFFFFFFFF);
	
	return 0;
	
}
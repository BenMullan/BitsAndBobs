/*
	MouseGone32.C
	Ben Mullan 2022
	-
	Compile using: cl.exe MouseGone32.C
*/

#include <Windows.H>
#pragma comment(lib, "User32.Lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd) {
	
	while (TRUE) {
		SetCursorPos(99999, 99999);
		Sleep(1);
	}
	
	return 0;
	
}
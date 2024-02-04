/*
	File:		Win32Window_Template.CPP - A Basic Template for a (Pure) Win32 GUI Program
	Compile:	cl.exe Win32Window.CPP
	Author:		Ben Mullan 2021

	Note:
		- A "Handle" is a Windows-Managed Pointer
		- &Variable means "Get the Memory Address of Variable (I.e. Get a Pointer to that Variable)" (AddressOf)
		- *Pointer means "Get the Variable that Pointer is Pointing to" (Dereferance)
*/


/* Paste in the Contents of the Windows.H File */
#include <Windows.H>

/* Tell the Linker to statically link the required Static Libraries */
#pragma comment(lib, "User32.Lib")	/* The Windows User API */
#pragma comment(lib, "GDI32.Lib")	/* The Windows Graphics API */

/* The WindowProcess, which Windows Calls every time the Program needs to know something */
LRESULT CALLBACK WndProc(HWND _HandleToTheWindow, UINT _EventWhichHasOccoured, WPARAM wParam, LPARAM lParam) {
	
	/* Holds Graphics Infomation for Painting the Window */
	HDC _HandleToDeviceContext;

	/* Used to paint the client area of a window */
	PAINTSTRUCT _PaintStruct;

	/* The Inner Portion of the Window */
	RECT _ClientRectangle;

	/* Windows has just called this function because an Event has occoured;
	Determine which Event has just occoured... */
	switch (_EventWhichHasOccoured) {

		/* The Window has just been Created */
		case WM_CREATE:
			return 0;

		/* Windows Wishes to Paint the Window's Contents again,
		E.g. because part of it was covered, and has just been shown again */
		case WM_PAINT:
			_HandleToDeviceContext = BeginPaint(_HandleToTheWindow, &_PaintStruct);
			GetClientRect(_HandleToTheWindow, &_ClientRectangle);

			/* The | deliminated flags at the end are all int's which are power's of 2. Hence the bitwise OR combines them into one int */
			DrawText(_HandleToDeviceContext, TEXT("Window Text"), -1, &_ClientRectangle, (DT_SINGLELINE | DT_CENTER | DT_VCENTER));
			EndPaint(_HandleToTheWindow, &_PaintStruct);
			return 0;

		/* The Window is to be "Destroyed" */
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	/* None of those cases were matched, so return this instead */
	return DefWindowProc(_HandleToTheWindow, _EventWhichHasOccoured, wParam, lParam);
}

/* The EntryPoint to the Program (ReturnType = int; CallingConvention = WINAPI; FunctionName = WinMain) */
int WINAPI WinMain(HINSTANCE _HandleToCurrentInstanceOfProgram, HINSTANCE _HandleToPreviousInstanceOfProgram, PSTR _PointerToCommandLineArguments, int _DesiredWindowState) {

	TCHAR _ProgramName[] = TEXT("WindowsProgram");

	HWND _HandleToTheWindow;
	MSG msg;

	WNDCLASS _WindowClass;
	
	/* The | deliminated flags at the end are all int's which are power's of 2. Hence the bitwise OR combines them into one int */
	_WindowClass.style = (CS_HREDRAW | CS_VREDRAW);
	_WindowClass.lpfnWndProc = WndProc;
	_WindowClass.cbClsExtra = 0;
	_WindowClass.cbWndExtra = 0;
	_WindowClass.hInstance = _HandleToCurrentInstanceOfProgram;
	_WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	_WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	_WindowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	_WindowClass.lpszMenuName = NULL;
	_WindowClass.lpszClassName = _ProgramName;

	if (!RegisterClass(&_WindowClass)) {
		MessageBox(NULL, TEXT("This program requires a version of Windows NT."), _ProgramName, MB_ICONERROR);
		return 0;
	}

	_HandleToTheWindow = CreateWindow(
		_ProgramName,						/* Program Name */
		TEXT("Window Title"),				/* Window Caption */
		WS_OVERLAPPEDWINDOW,				/* Window Style */
		CW_USEDEFAULT,						/* Initial X Position */
		CW_USEDEFAULT,						/* Initial Y Position */
		CW_USEDEFAULT,						/* Initial X Size */
		CW_USEDEFAULT,						/* Initial Y Size */
		NULL,								/* Parent Window Handle */
		NULL,								/* Window Menu Handle */
		_HandleToCurrentInstanceOfProgram,	/* Program Instance Handle */
		NULL								/*	Creation Parameters */
	);

	ShowWindow(_HandleToTheWindow, _DesiredWindowState);
	UpdateWindow(_HandleToTheWindow);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
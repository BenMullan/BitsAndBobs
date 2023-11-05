/*
	MaxVol.C - Sets the System Volume to 100%. (By means of a non-keystroke-injection method)
	Ben Mullan 2022
	-
	Compile using: cl.exe MaxVol.C
*/

#include <Windows.H>
#include <MMDeviceAPI.H>
#include <EndPointVolume.H>

#pragma comment(lib, "User32.Lib")
#pragma comment(lib, "OLE32.Lib")

bool ChangeVolume(double nVolume) {

	HRESULT hr = NULL;
	bool decibels = false;
	bool scalar = false;
	double newVolume = nVolume;

	CoInitialize(NULL);
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_INPROC_SERVER, 
		__uuidof(IMMDeviceEnumerator),
		(LPVOID*)&deviceEnumerator
	);
	IMMDevice* defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	IAudioEndpointVolume* endpointVolume = NULL;
	hr = defaultDevice->Activate(
		__uuidof(IAudioEndpointVolume), 
		CLSCTX_INPROC_SERVER,
		NULL,
		(LPVOID*)&endpointVolume
	);
	defaultDevice->Release();
	defaultDevice = NULL;

	float currentVolume = 0;
	endpointVolume->GetMasterVolumeLevel(&currentVolume);
	
	hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
	hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
	
	endpointVolume->Release();
	CoUninitialize();

	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd) {
	
	ChangeVolume(1.0);
	return 0;
	
}
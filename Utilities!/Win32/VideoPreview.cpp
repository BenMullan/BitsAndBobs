// file:    VideoPreview.cpp
// exec:    "E:\Programs\_Programming\GCC\bin\g++.exe" VideoPreview.cpp -lole32 -luser32 -lgdi32 -lquartz -lstrmiids -luuid
// author:  Ben Mullan (c) 2025, + fixes by `Gemini`

#include <windows.h>
#include <dshow.h>

// Globals for COM interfaces
static IGraphBuilder        *g_pGraph        = nullptr;
static ICaptureGraphBuilder2 *g_pCapture     = nullptr;
static IMediaControl        *g_pControl      = nullptr;
static IBaseFilter          *g_pVideoCapture = nullptr;
static IVideoWindow         *g_pVideoWindow  = nullptr;

// FIX: Forward declaration for Cleanup()
void Cleanup();

// Initialize DirectShow graph and start preview
void InitVideoPreview(HWND hwnd) {
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        OutputDebugStringW(L"CoInitialize failed.\n");
        return;
    }

    // FIX: Manual expansion of IID_PPV_ARGS
    // Create the Filter Graph Manager
    hr = CoCreateInstance(
        CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER,
        IID_IGraphBuilder, (void**)&g_pGraph
    );
    if (FAILED(hr)) { OutputDebugStringW(L"CoCreateInstance(FilterGraph) failed.\n"); return; }

    // Create the Capture Graph Builder
    hr = CoCreateInstance(
        CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER,
        IID_ICaptureGraphBuilder2, (void**)&g_pCapture
    );
    if (FAILED(hr)) { OutputDebugStringW(L"CoCreateInstance(CaptureGraphBuilder2) failed.\n"); return; }

    g_pCapture->SetFiltergraph(g_pGraph);

    // Enumerate video capture devices
    ICreateDevEnum *pDevEnum = nullptr;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER,
                          IID_ICreateDevEnum, (void**)&pDevEnum);
    if (FAILED(hr)) { OutputDebugStringW(L"CoCreateInstance(SystemDeviceEnum) failed.\n"); return; }

    IEnumMoniker *pEnum = nullptr;
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr == S_OK)
    {
        IMoniker *pMoniker = nullptr;
        if (pEnum->Next(1, &pMoniker, nullptr) == S_OK)
        {
            // Bind the first device
            // FIX: Manual expansion for BindToObject call
            hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void**)&g_pVideoCapture);
            if (SUCCEEDED(hr)) {
                hr = g_pGraph->AddFilter(g_pVideoCapture, L"Video Capture");
            }
            pMoniker->Release();
        }
        if (pEnum) pEnum->Release();
    }
    if (pDevEnum) pDevEnum->Release();

    if (FAILED(hr) || !g_pVideoCapture) {
        OutputDebugStringW(L"Failed to find or add video capture device.\n");
        Cleanup();
        return;
    }

    // Render the preview pin (chooses default renderer)
    hr = g_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
                                  g_pVideoCapture, nullptr, nullptr);
    if (FAILED(hr)) { OutputDebugStringW(L"RenderStream failed.\n"); Cleanup(); return; }

    // Grab the video window interface and attach to our Win32 HWND
    // FIX: Manual expansion for QueryInterface
    hr = g_pGraph->QueryInterface(IID_IVideoWindow, (void**)&g_pVideoWindow);
    if (FAILED(hr)) { OutputDebugStringW(L"QueryInterface(IVideoWindow) failed.\n"); Cleanup(); return; }

    g_pVideoWindow->put_Owner((OAHWND)hwnd);
    g_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
    g_pVideoWindow->put_Visible(OATRUE);

    // Size it to fill the client area
    RECT rc;
    GetClientRect(hwnd, &rc);
    g_pVideoWindow->SetWindowPosition(0, 0, rc.right, rc.bottom);

    // Run the graph
    // FIX: Manual expansion for QueryInterface
    hr = g_pGraph->QueryInterface(IID_IMediaControl, (void**)&g_pControl);
    if (FAILED(hr)) { OutputDebugStringW(L"QueryInterface(IMediaControl) failed.\n"); Cleanup(); return; }
    
    hr = g_pControl->Run();
    if (FAILED(hr)) { OutputDebugStringW(L"MediaControl->Run() failed.\n"); Cleanup(); return; }
}

// Cleanup DirectShow objects
void Cleanup()
{
    if (g_pControl)        { g_pControl->Stop(); g_pControl->Release(); g_pControl = nullptr; }
    if (g_pVideoWindow)    { 
        g_pVideoWindow->put_Visible(OAFALSE);
        g_pVideoWindow->put_Owner((OAHWND)NULL); 
        g_pVideoWindow->Release(); 
        g_pVideoWindow = nullptr; 
    }
    if (g_pVideoCapture)   { g_pVideoCapture->Release(); g_pVideoCapture = nullptr; }
    if (g_pCapture)        { g_pCapture->Release(); g_pCapture = nullptr; }
    if (g_pGraph)          { g_pGraph->Release(); g_pGraph = nullptr; }
    CoUninitialize();
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg)
    {
    case WM_CREATE:
        InitVideoPreview(hwnd);
        return 0;
    case WM_SIZE:
        if (g_pVideoWindow)
        {
            RECT rc;
            GetClientRect(hwnd, &rc);
            g_pVideoWindow->SetWindowPosition(0, 0, rc.right, rc.bottom);
        }
        return 0;
    case WM_DESTROY:
        Cleanup();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, w, l);
}

// Entry point
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    // Register window class
    WNDCLASSEXW wc = { sizeof(wc), CS_HREDRAW|CS_VREDRAW, WndProc,
                      0, 0, hInst, nullptr, LoadCursor(nullptr, IDC_ARROW),
                      nullptr, nullptr, L"VideoPreviewClass", nullptr };
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create and show window
    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"Video Preview",
                                WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                640, 480, nullptr, nullptr, hInst, nullptr);
    if (!hwnd) {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}
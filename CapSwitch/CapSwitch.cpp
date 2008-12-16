/*
 * CapSwitch.cpp
 *
 * CapSwitch program allows to change locale by pressing CapsLock key.
 * To toggle capitals mode on/off use lshift+rshift.
 *
 * Implemented as global keyboard hook.
 */

#include "stdafx.h"
#include "CapSwitch.h"
#include "Hook/Hook.h"

#define CAPSWITCH_CLASS_NAME L"CapSwitchClass"
#define CAPSWITCH_WINDOW_NAME L"CapSwitch"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    int retval = 0;
    HWND hWnd = 0;

    try {

        // Register the window class for hidden window.

        WNDCLASSEX wcx; 

        wcx.cbSize = sizeof(wcx);
        wcx.style = 0;
        wcx.lpfnWndProc = WndProc;
        wcx.cbClsExtra = 0;
        wcx.cbWndExtra = 0;
        wcx.hInstance = hInstance;
        wcx.hIcon = NULL;
        wcx.hCursor = NULL;
        wcx.hbrBackground = NULL;
        wcx.lpszMenuName =  NULL;
        wcx.lpszClassName = CAPSWITCH_CLASS_NAME;
        wcx.hIconSm = NULL;
     
        if (RegisterClassEx(&wcx) == NULL) {
            throw 1;
        }

        // Create hidden window.

        hWnd = CreateWindow( 
            CAPSWITCH_CLASS_NAME,
            CAPSWITCH_WINDOW_NAME,
            0,                   // hidden window
            0, 0, 0, 0,
            (HWND) NULL,         // no owner window 
            (HMENU) NULL,        // use class menu 
            hInstance,
            (LPVOID) NULL);      // no window-creation data 

        if (hWnd == NULL) {
            throw 2;
        }

        HMODULE hDll = GetModuleHandle(L"Hook.dll");

        if (hDll == NULL) {
            throw 3;
        }

        if (KbHookInit(hWnd, hDll) == false) {
            throw 4;
        }

        // Message loop.

        MSG msg;

        while (GetMessage(&msg, NULL, 0, 0) != 0) {
            DispatchMessage(&msg);
        }
    }

    catch (int err) {
        retval = err;
    }

    KbHookShutdown();
    DestroyWindow(hWnd);
    UnregisterClass(CAPSWITCH_CLASS_NAME, hInstance);

    return retval;
}

int ScanCode(LPARAM lParam)
{
    return (lParam << 8) >> 24;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static bool debug = false;

    if (uMsg == WM_CAPSWITCH_LOCALE_CHANGE) {
        if (debug) {
            wchar_t buf[30];
            wsprintf(buf, L"LocaleChange  prev=%d\n", wParam);
            OutputDebugString(buf);
        }
    } else if (uMsg == WM_CAPSWITCH_DEBUG) {
        if (debug) {
            wchar_t buf[50];
            wsprintf(buf, L"w=%d  l=%d  scan=%d\n", wParam, lParam, ScanCode(lParam));
            OutputDebugString(buf);
        }
    } else if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
    } else {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

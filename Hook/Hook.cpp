// Hook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Hook.h"
#include "CapSwitch/CapSwitch.h"

#define SHARED_SECTION_NAME "shdata"
#define DLL_SHARED __declspec(allocate(SHARED_SECTION_NAME))

#pragma section(SHARED_SECTION_NAME, read, write, shared)
DLL_SHARED HWND hSrvWnd = 0;
DLL_SHARED HHOOK hKbHook = 0;
DLL_SHARED bool bFakeCaps = false;

#define HIGH_BIT_SET 0x8000
#define LOW_BIT_SET 0x0001

int ScanCode(LPARAM lParam)
{
    return (lParam << 8) >> 24;
}

bool BeingReleased(LPARAM lParam)
{
    return ((lParam & 0x80000000) != 0);
}

bool BeingPressed(LPARAM lParam)
{
    return !BeingReleased(lParam);
}

#define SCAN_CODE(lParam) ((lParam << 8) >> 24)

void CancelCaps()
{
    keybd_event(
        VK_CAPITAL,
        0xFF,
        KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
        0);

    keybd_event(
        VK_CAPITAL,
        0xFF,
        KEYEVENTF_EXTENDEDKEY,
        0);

    keybd_event(
        VK_CAPITAL,
        0xFF,
        KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
        0);
}

void ToggleCapsLock()
{
    keybd_event(
        VK_CAPITAL,
        0xFF,
        KEYEVENTF_EXTENDEDKEY,
        0);

    keybd_event(
        VK_CAPITAL,
        0xFF,
        KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
        0);
}

LRESULT CALLBACK KbHookFn(int code, WPARAM wParam, LPARAM lParam)
{
    if (code < 0) {
        return CallNextHookEx(hKbHook, code, wParam, lParam);
    }

    PostMessage(hSrvWnd, WM_CAPSWITCH_DEBUG, wParam, lParam);

    int scanCode = ScanCode(lParam);

    if (scanCode == 54) { // RSHIFT
        if (GetKeyState(VK_LSHIFT) & HIGH_BIT_SET) {
            ToggleCapsLock();
            return 1;
         }
    } else if (scanCode == 42) { // LSHIFT
        if (GetKeyState(VK_RSHIFT) & HIGH_BIT_SET) {
            ToggleCapsLock();
            return 1;
        }
    } else if (scanCode == 58) { // CAPSLOCK
        if (BeingPressed(lParam)) {
            ActivateKeyboardLayout((HKL) HKL_NEXT, KLF_SETFORPROCESS);
            PostMessage(hSrvWnd, WM_CAPSWITCH_LOCALE_CHANGE, 0, 0);
            CancelCaps();
        }
        return 1;
    }

    return CallNextHookEx(hKbHook, code, wParam, lParam);
}

HOOK_API bool KbHookInit(HWND hWnd, HMODULE hDll)
{
    hSrvWnd = hWnd;

    hKbHook = SetWindowsHookEx(
        WH_KEYBOARD,
        KbHookFn,
        (HINSTANCE) hDll,
        0);                 // hook is set globally

    return (hKbHook != 0);
}

HOOK_API void KbHookShutdown()
{
    UnhookWindowsHookEx(hKbHook);
}

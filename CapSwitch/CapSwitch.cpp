/*
 * CapSwitch.cpp
 *
 * CapSwitch program allows to change locale by pressing CapsLock key.
 * To toggle capitals mode on/off use lshift+rshift.
 *
 * Implemented as global keyboard hook.
 */

#pragma comment(linker,"/NODEFAULTLIB")
#pragma comment(linker,"/ENTRY:WinMain")

#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#define HIGH_BIT_SET 0x8000
#define LOW_BIT_SET 0x0001

HHOOK hKbHook = NULL;

void CancelCapsOnWindow(HWND hWnd)
{
	PostMessage(hWnd, WM_KEYUP, 20, 0);
	PostMessage(hWnd, WM_KEYDOWN, 20, 0);
	PostMessage(hWnd, WM_KEYUP, 20, 0);
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

LRESULT CALLBACK LlKbHookFn(int code, WPARAM wParam, LPARAM lParam)
{
	if (code < 0 || code != HC_ACTION)
    {
		return CallNextHookEx(hKbHook, code, wParam, lParam);
	}

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{
		KBDLLHOOKSTRUCT *kbDllHookStruct = (KBDLLHOOKSTRUCT *)lParam;

		//wchar_t buf[100];
		//wsprintf(buf, L"w=%d scan=%u vk=%u flags=%u extra=%u\n", wParam, kbDllHookStruct->scanCode, kbDllHookStruct->vkCode, kbDllHookStruct->flags, kbDllHookStruct->dwExtraInfo);
		//OutputDebugString(buf);

		int scanCode = kbDllHookStruct->scanCode;

		if (scanCode == 54) // RSHIFT
		{ 
			if (GetKeyState(VK_LSHIFT) & HIGH_BIT_SET)
			{
				ToggleCapsLock();
				return 1;
			}
		}
		else if (scanCode == 42) // LSHIFT
		{ 
			if (GetKeyState(VK_RSHIFT) & HIGH_BIT_SET)
			{
				ToggleCapsLock();
				return 1;
			}
		}
		else if (scanCode == 58) // CAPSLOCK
		{
			HWND hWnd = GetForegroundWindow();
			PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_FORWARD, 0);
			CancelCapsOnWindow(hWnd);
			return 1;
		}
	}

	return CallNextHookEx(hKbHook, code, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hKbHook = SetWindowsHookEx(WH_KEYBOARD_LL, LlKbHookFn, NULL, 0);

	if (hKbHook == NULL)
	{
		return 1;
	}

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hKbHook);

	return 0;
}


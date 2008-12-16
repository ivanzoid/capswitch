#ifndef __Hook_h__
#define __Hook_h__

// HOOK_EXPORTS is set in settings for dll project.
#ifdef HOOK_EXPORTS
#define HOOK_API __declspec(dllexport)
#else
#define HOOK_API __declspec(dllimport)
#endif

HOOK_API bool KbHookInit(HWND hWnd, HMODULE hDll);
HOOK_API void KbHookShutdown();

#endif // __Hook_h__

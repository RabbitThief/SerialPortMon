#pragma once

#define WM_RESTART	(WM_USER + 1239)
#define WM_MM_TIMER	(WM_USER + 1250)

extern void SendRestartMessage (char *WindowClass, char *Title);
extern bool ApplicationAlreadyExist (char *WindowClass, char *Title);

extern PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc);

extern CString GetVersionInfo(HMODULE hLib, CString csEntry);

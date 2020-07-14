#include "stdafx.h"
#include "etc.h"

void SendRestartMessage (char *WindowClass, char *Title)
{
	HWND hPrevWnd = FindWindow(WindowClass, Title);
	if (!hPrevWnd) return;

	// Command line argument를 hPrevWnd 윈도우로 전송한다.
	char *cmd = GetCommandLine();
	int n_cmd = strlen(cmd)+1;
	char *buff = (char *)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, n_cmd);

	strcpy (buff, cmd);

	COPYDATASTRUCT tip;
	tip.dwData = 0;
	tip.cbData = n_cmd;
	tip.lpData = buff;
	
	SendMessage (hPrevWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&tip);
	
	::HeapFree(::GetProcessHeap(), 0, buff);

	PostMessage (hPrevWnd, WM_RESTART, 0, 0);
	FlashWindow (hPrevWnd, TRUE);
}

bool ApplicationAlreadyExist (char *WindowClass, char *Title)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, Title);   // Create mutex
	switch(GetLastError()) {
	case ERROR_SUCCESS:
		break;
	case ERROR_ALREADY_EXISTS:
		SendRestartMessage (WindowClass, Title);
		return true;
	default:
		return false;
	}
	return false;
}

PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG  len;
	ULONG  argc;
	CHAR   a;
	ULONG  i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED, i + (len+2)*sizeof(CHAR));
	_argv = (PCHAR)(((PUCHAR)argv)+i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while( a = CmdLine[i] ) {
		if(in_QM) {
			if(a == '\"') {
				in_QM = FALSE;
			} else {
				_argv[j] = a;
				j++;
			}
		} 
		else {
			switch(a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if(in_SPACE) {
					argv[argc] = _argv+j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if(in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if(in_SPACE) {
					argv[argc] = _argv+j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = FALSE;
				break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}


#pragma comment (lib, "Version.lib")

CString GetVersionInfo(HMODULE hLib, CString csEntry)
{
	CString csRet;

	if (hLib == NULL) hLib = AfxGetResourceHandle();

	HRSRC hVersion = FindResource( hLib, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
	if (hVersion != NULL) {
		HGLOBAL hGlobal = LoadResource( hLib, hVersion );
		if ( hGlobal != NULL) {

			LPVOID versionInfo = LockResource(hGlobal);
			if (versionInfo != NULL) {
				DWORD vLen,langD;
				BOOL retVal;

				LPVOID retbuf=NULL;
				static char fileEntry[256];
				sprintf(fileEntry,_T("\\VarFileInfo\\Translation"));

				retVal = VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen);
				if (retVal && vLen==4) {
					memcpy(&langD,retbuf,4);
					sprintf(fileEntry, _T("\\StringFileInfo\\%02X%02X%02X%02X\\%s"),
						(langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24,
						(langD & 0xff0000)>>16, csEntry);
				}
				else
					sprintf(fileEntry, "\\StringFileInfo\\%04X04B0\\%s", GetUserDefaultLangID(), (LPCTSTR)csEntry);

				if (VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen))
					csRet = (char*)retbuf;
			}
		}

		UnlockResource( hGlobal );
		FreeResource( hGlobal );
	}

	return csRet;
}

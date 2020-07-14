#include "stdafx.h"
#include "shortkey.h"
#include "../resource.h"
#include "../SerialPortMonDlg.h"

void sShortkey::Start (char *str, int len, int delay, int repeat)
{
	if (_count >= 0) return;

	Stop ();

	_str = new char [len+1];
	_strLen = len;
	memcpy (_str, str, len);
	_str[len] = '\0';

	_delay = delay;
	_repeat = repeat;

	_count = 0;
	_to = GetTickCount ();
}

void sShortkey::Stop ()
{
	_count = -1;
	_to = 0;
	
	if (_str) delete [] _str;
	_str = NULL;
	_strLen = 0;
}

void sShortkey::Step (DWORD t)
{
	if (_count < 0) return;

	if (_to <= t) {
		if (_count < _repeat) {
			_to = t + _delay;
			_count ++;

			CSerialPortMonDlg *mainDlg = dynamic_cast<CSerialPortMonDlg *>(AfxGetApp()->m_pMainWnd);
			if (mainDlg) mainDlg->SendMessageSerial (_str, _strLen);
		}
		else Stop ();
	}
}

bool sShortkey::IsRun ()
{
	return (0 < _count);
}

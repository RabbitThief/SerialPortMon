#include "stdafx.h"
#include "EditKeyinScroll.h"
#include "escape_seq.h"


BEGIN_MESSAGE_MAP(CEditKeyinScroll, CEdit)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CEditKeyinScroll::CEditKeyinScroll()
{
	_hexa = false;
	_cursor = 0;
	_keyinText.reserve (1024);
}

CEditKeyinScroll::~CEditKeyinScroll() 
{
}

void CEditKeyinScroll::AddScrollText(const char *text)
{
	if (_keyinText.size()) {
		if (_keyinText[_keyinText.size()-1] != text) {
			_keyinText.push_back (string(text));
		}
	}
	else {
		_keyinText.push_back (string(text));
	}
	_cursor = _keyinText.size();
}

void CEditKeyinScroll::SetWindowTextStr (string &str)
{
	if (!_hexa) {
		SetWindowText (str.c_str());
	}
	else {
		char *hexachar = new char [3*str.length () + 10];
		int n = escseq2hexachar (str.c_str(), hexachar, 3*str.length () + 9);
		hexachar[n] = '\0';

		SetWindowText (hexachar);
		delete [] hexachar;
	}
}

void CEditKeyinScroll::OnKey ()
{
	if (0 <= _cursor && _cursor < (int)_keyinText.size()) {
		SetWindowTextStr (_keyinText.at(_cursor));
		SetSel (0, -1);
	}
}

void CEditKeyinScroll::KeyDown ()
{
	if (_cursor < (int)_keyinText.size() - 1) ++_cursor;
	OnKey ();
}

void CEditKeyinScroll::KeyUp ()
{
	if (0 < _cursor) --_cursor;
	OnKey ();
}

void CEditKeyinScroll::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == VK_RETURN) {
		/*
		char buff[1024+1];
		GetWindowText (buff, 1024);
		buff[1024] = 0;

		_keyinText.push_back (string(buff));
		_cursor = _keyinText.size();
		*/
	}
	else if (nChar == VK_UP || nChar == VK_PRIOR) {
		if (0 < _cursor) --_cursor;
		OnKey ();
		return;
	}
	else if (nChar == VK_DOWN || nChar == VK_NEXT) {
		if (_cursor < (int)_keyinText.size() - 1) ++_cursor;
		OnKey ();
		return;
	}
	else if (nChar == VK_HOME) {
		_cursor = (int)_keyinText.size() - 1;
		OnKey ();
		return;
	}
	else if (nChar == VK_END) {
		_cursor = 0;
		OnKey ();
		return;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

#include "stdafx.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEditEx::CEditEx ()
{
}

void CEditEx::DoDataExchange(CDataExchange* pDX)
{
	CEdit::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
END_MESSAGE_MAP()


BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	// Intercept Ctrl + Z (Undo), Ctrl + X (Cut), Ctrl + C (Copy), Ctrl + V (Paste) and Ctrl + A (Select All)
	// before CEdit base class gets a hold of them.
	if (pMsg->message == WM_KEYDOWN && ::GetKeyState(VK_CONTROL) < 0)
		switch (pMsg->wParam) {
		//case 'Z': Undo();        return TRUE;
		//case 'X': Cut();         return TRUE;
		//case 'C': Copy();        return TRUE;
		//case 'V': Paste();       return TRUE;
		case 'A': SetSel(0, -1); return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

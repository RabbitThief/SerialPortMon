#pragma once
#include "afxwin.h"

class CEditEx : public CEdit {
public:
	CEditEx ();
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


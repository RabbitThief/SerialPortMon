#pragma once 
#include <afxwin.h>
#include <vector>
#include <string>

using namespace std;
 
class CEditKeyinScroll: public CEdit 
{
public:
	CEditKeyinScroll();
	virtual ~CEditKeyinScroll();
 
	void SetWindowTextStr (string &str);
	void AddScrollText(const char *text);

	void OnKey ();
	void KeyDown ();
	void KeyUp ();

	bool _hexa;

private:
	int _cursor;
	vector<string> _keyinText;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

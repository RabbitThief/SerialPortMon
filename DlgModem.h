#pragma once


class CDlgModem : public CDialog
{
	DECLARE_DYNAMIC(CDlgModem)

public:
	CDlgModem(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgModem();

	enum { IDD = IDD_DIALOG_MODEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	HBITMAP HB_LAMP_DIS;
	HBITMAP HB_LAMP_OFF;
	HBITMAP HB_LAMP_ON;

	HBITMAP SelectBitmap (bool connected, bool flag);
	BOOL IsObjDeleteable(HGDIOBJ hGdiObj);
	BOOL ReplaceStaticControlBitmap (int ControlID, HBITMAP hBitmap);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheckBreak();
	afx_msg void OnBnClickedCheckRts();
	afx_msg void OnBnClickedCheckDtr();
	afx_msg void OnBnClickedButtonXon();
	afx_msg void OnBnClickedButtonXoff();
};

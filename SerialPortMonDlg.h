#pragma once
#include "afxwin.h"
#include "EditKeyinScroll.h"
#include "EditEx.h"
#include "EditLogB.h"
#include "DlgModem.h"
#include "DlgUdp.h"
#include "HyperLink.h"
#include "CommThread.h"

class CDlgShortkey;

class CSerialPortMonDlg : public CDialog
{
public:
	CSerialPortMonDlg(CWnd* pParent = NULL);	// standard constructor
	~CSerialPortMonDlg();

	void SendMessageSerial (char *msg, int msgLen);
	void LogMessage (const char *msg, int msgLen, bool send);

	enum { IDD = IDD_SERIALPORTMON_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	HICON m_hIcon;

private:
	int			_connectPrev;
	CFont		_fontFixedsys; 
	CHyperLink	_staticHomepage;
	CEditLogB	_editMessage;
	CEditKeyinScroll _editInput;
	CDlgShortkey    *_dlgShortkey;
	CDlgModem		 _dlgModem;
	CDlgUdp			 _dlgUdp;
	CCommThread		*_commThread;

	void DlgInScreen ();
	void EnableWindowItem (BOOL enable);

	void WordWrap ();
	void SetTitle ();

	void CloseConnection ();
	bool OpenConnection ();
	int GetDlgItemStr (int nIDEdit, char *str, int maxStr);
	void DisplayCannotConnectMsg ();

	CString GetDisplayTime (int option);
	int ReplaceCharEscape (char *textchar, const char *msg, int msglen);
	int ReplaceCharCRLF   (char *textchar, const char *msg, int msglen);
	int ReplaceCharASCII  (char *textchar, const char *msg, int msglen);
	int ReplaceCharAs     (char *textchar, char replace, const char *msg, int msglen);

	void ChangeDlgItemTextFormat (int nIDEdit, bool hexa);
	void OnBnClickedRadioSuffix();
	void OnBnClickedRadioInput();
	bool OnBnClickedOkCancel();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonShortkey();
	afx_msg void OnBnClickedButtonConfiguration();
	afx_msg void OnBnClickedButtonConnectClose();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedRadioMessageHex();
	afx_msg void OnBnClickedRadioRecordAscii();
	afx_msg void OnBnClickedRadioInputHex();
	afx_msg void OnBnClickedRadioInputAscii();
	afx_msg void OnDeltaposSpinSend(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckDisplaySend();
	afx_msg void OnBnClickedCheckDisplayRecv();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

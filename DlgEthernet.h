#pragma once
#include "afxcmn.h"


class CDlgEthernet : public CDialog
{
	DECLARE_DYNAMIC(CDlgEthernet)

public:
	CDlgEthernet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEthernet();

	void SetConnectionMode (int mode);

	enum { IDD = IDD_DIALOG_ETHERNET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	CIPAddressCtrl _remoteIpAddr;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckBroadcast();
	afx_msg void OnBnClickedCancel();
};

#pragma once


// CDlgUdp dialog

class CDlgUdp : public CDialog
{
	DECLARE_DYNAMIC(CDlgUdp)

public:
	CDlgUdp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUdp();

// Dialog Data
	enum { IDD = IDD_DIALOG_UDP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};

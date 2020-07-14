#pragma once

class CDlgConfiguration : public CDialog
{
	DECLARE_DYNAMIC(CDlgConfiguration)

public:
	CDlgConfiguration(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgConfiguration();

	enum { IDD = IDD_DIALOG_CONFIGURATION };

	CSpinButtonCtrl _spinBufferLines;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnCbnSelchangeComboReplaceChar();
};

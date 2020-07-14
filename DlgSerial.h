#pragma once


class CDlgSerial : public CDialog
{
	DECLARE_DYNAMIC(CDlgSerial)

public:
	CDlgSerial(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSerial();

	enum { IDD = IDD_DIALOG_SERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:


public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

int GetComSpeedIndex(int value);
int GetComSpeedValue(int index);

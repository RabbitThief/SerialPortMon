#pragma once
#include "DlgSerial.h"
#include "DlgEthernet.h"


class CDlgConnection : public CDialog
{
	DECLARE_DYNAMIC(CDlgConnection)

public:
	CDlgConnection(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgConnection();

	enum { IDD = IDD_DIALOG_CONNECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	CDlgSerial		_dlgSerial;		// nested dialog
	CDlgEthernet	_dlgEthernet;	// nested dialog

	void OnBnClickedRadio();


	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioUdp();
	afx_msg void OnBnClickedRadioTcps();
	afx_msg void OnBnClickedRadioTcpc();
	afx_msg void OnBnClickedRadioCom();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

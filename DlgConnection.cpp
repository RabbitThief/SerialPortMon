#include "stdafx.h"
#include "SerialPortMon.h"
#include "DlgConnection.h"
#include "AppIni.h"


IMPLEMENT_DYNAMIC(CDlgConnection, CDialog)

CDlgConnection::CDlgConnection(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConnection::IDD, pParent), _dlgSerial(this), _dlgEthernet(this)
{
}

CDlgConnection::~CDlgConnection()
{
}

void CDlgConnection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgConnection, CDialog)
	ON_BN_CLICKED(IDC_RADIO_UDP, &CDlgConnection::OnBnClickedRadioUdp)
	ON_BN_CLICKED(IDC_RADIO_TCPS, &CDlgConnection::OnBnClickedRadioTcps)
	ON_BN_CLICKED(IDC_RADIO_TCPC, &CDlgConnection::OnBnClickedRadioTcpc)
	ON_BN_CLICKED(IDC_RADIO_COM, &CDlgConnection::OnBnClickedRadioCom)
	ON_BN_CLICKED(IDOK, &CDlgConnection::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgConnection::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDlgConnection::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	_dlgSerial.Create (IDD_DIALOG_SERIAL, this);
	_dlgSerial.SetWindowPos (NULL, 2, 52, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	_dlgSerial.ShowWindow (SW_HIDE);

	_dlgEthernet.Create (IDD_DIALOG_ETHERNET, this);
	_dlgEthernet.SetWindowPos (NULL, 2, 52, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	_dlgEthernet.ShowWindow (SW_HIDE);

	CheckDlgButton (IDC_RADIO_UDP,  (g_ini.options.CONNECTION_TYPE == CONNECTION_UDP ) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_TCPS, (g_ini.options.CONNECTION_TYPE == CONNECTION_TCPS) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_TCPC, (g_ini.options.CONNECTION_TYPE == CONNECTION_TCPC) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_COM,  (g_ini.options.CONNECTION_TYPE == CONNECTION_COM ) ? BST_CHECKED : BST_UNCHECKED);

	OnBnClickedRadio ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConnection::OnBnClickedRadio() 
{	
	if      (IsDlgButtonChecked (IDC_RADIO_UDP )  == BST_CHECKED) g_ini.options.CONNECTION_TYPE = CONNECTION_UDP ;
	else if (IsDlgButtonChecked (IDC_RADIO_TCPS)  == BST_CHECKED) g_ini.options.CONNECTION_TYPE = CONNECTION_TCPS;
	else if (IsDlgButtonChecked (IDC_RADIO_TCPC)  == BST_CHECKED) g_ini.options.CONNECTION_TYPE = CONNECTION_TCPC;
	else if (IsDlgButtonChecked (IDC_RADIO_COM )  == BST_CHECKED) g_ini.options.CONNECTION_TYPE = CONNECTION_COM ;

	switch (g_ini.options.CONNECTION_TYPE) {
	case CONNECTION_UDP : _dlgSerial.ShowWindow (SW_HIDE); _dlgEthernet.ShowWindow (SW_SHOW); break;
	case CONNECTION_TCPS: _dlgSerial.ShowWindow (SW_HIDE); _dlgEthernet.ShowWindow (SW_SHOW); break;
	case CONNECTION_TCPC: _dlgSerial.ShowWindow (SW_HIDE); _dlgEthernet.ShowWindow (SW_SHOW); break;
	case CONNECTION_COM : _dlgSerial.ShowWindow (SW_SHOW); _dlgEthernet.ShowWindow (SW_HIDE); break;
	}
	_dlgEthernet.SetConnectionMode (g_ini.options.CONNECTION_TYPE); 
}

void CDlgConnection::OnBnClickedRadioUdp() {	OnBnClickedRadio (); }
void CDlgConnection::OnBnClickedRadioTcps(){	OnBnClickedRadio (); }
void CDlgConnection::OnBnClickedRadioTcpc(){	OnBnClickedRadio (); }
void CDlgConnection::OnBnClickedRadioCom() {	OnBnClickedRadio (); }

void CDlgConnection::OnBnClickedOk()
{
	_dlgSerial.OnBnClickedOk();
	_dlgEthernet.OnBnClickedOk();

	OnOK();
}

void CDlgConnection::OnBnClickedCancel()
{
	OnCancel();
}

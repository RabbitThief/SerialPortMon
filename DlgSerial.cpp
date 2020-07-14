#include "stdafx.h"
#include "typedef.h"
#include "SerialPortMon.h"
#include "DlgSerial.h"
#include "AppIni.h"

static int _comSpeed[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 };

int GetComSpeedIndex(int value)
{
	for (int i=0; i<sizeof(_comSpeed)/sizeof(int); i++) {
		if (_comSpeed[i] == value) return i;
	}
	return 7;	// 115200
}

int GetComSpeedValue(int index)
{
	if (0 <= index && index < sizeof(_comSpeed)/sizeof(int)) {
		return _comSpeed[index];
	}
	return _comSpeed[7];	// 115200
}


IMPLEMENT_DYNAMIC(CDlgSerial, CDialog)

CDlgSerial::CDlgSerial(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSerial::IDD, pParent)
{

}

CDlgSerial::~CDlgSerial()
{
}

void CDlgSerial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSerial, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSerial::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSerial::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDlgSerial::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	int sel = -1;
	SendDlgItemMessage (IDC_COMBO_PORT, CB_INITSTORAGE, 10, 100);
	SendDlgItemMessage (IDC_COMBO_PORT,  WM_SETREDRAW, 0, 0);
	for (int i=0; i<256; i++) {
		char name[MAX_PATH];
		sprintf (name, "\\\\.\\COM%d", i);

		// COM 포트를 열어본다. 열리면 COM 포트가 있는 것이다.
		HANDLE h = CreateFile (name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
		if (h != INVALID_HANDLE_VALUE){
			CloseHandle (h);

			SendDlgItemMessage (IDC_COMBO_PORT, CB_ADDSTRING, 0, (LPARAM)name);
		}
	}
	SetDlgItemText (IDC_COMBO_PORT, g_ini.comPort);
	SendDlgItemMessage (IDC_COMBO_PORT,  WM_SETREDRAW, 1, 0);

	SetDlgItemInt      (IDC_COMBO_BPS,		g_ini.comBps, FALSE);
	SendDlgItemMessage (IDC_COMBO_DATABITS, CB_SETCURSEL, g_ini.comDatabits - 5);
	SendDlgItemMessage (IDC_COMBO_PARITY,	CB_SETCURSEL, g_ini.comParity);
	SendDlgItemMessage (IDC_COMBO_STOPBITS, CB_SETCURSEL, g_ini.comStopbits);
	SendDlgItemMessage (IDC_COMBO_FLOWCTL,	CB_SETCURSEL, g_ini.comFlowcontrol);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSerial::OnBnClickedOk()
{
	GetDlgItemText (IDC_COMBO_PORT, g_ini.comPort, MAX_PATH);

	g_ini.comBps		= GetDlgItemInt (IDC_COMBO_BPS, 0, FALSE);
	g_ini.comDatabits	= (uchar)(SendDlgItemMessage (IDC_COMBO_DATABITS,	CB_GETCURSEL) + 5);
	g_ini.comParity		= (uchar)(SendDlgItemMessage (IDC_COMBO_PARITY,		CB_GETCURSEL)); 
	g_ini.comStopbits	= (uchar)(SendDlgItemMessage (IDC_COMBO_STOPBITS,	CB_GETCURSEL)); 
	g_ini.comFlowcontrol= (uchar)(SendDlgItemMessage (IDC_COMBO_FLOWCTL,	CB_GETCURSEL)); 

	OnOK();
}

void CDlgSerial::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//OnCancel();
}

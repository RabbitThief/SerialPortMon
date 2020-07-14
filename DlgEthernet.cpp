#include "stdafx.h"
#include "SockCommon.h"
#include "SerialPortMon.h"
#include "DlgEthernet.h"
#include "AppIni.h"


IMPLEMENT_DYNAMIC(CDlgEthernet, CDialog)

CDlgEthernet::CDlgEthernet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEthernet::IDD, pParent)
{
}

CDlgEthernet::~CDlgEthernet()
{
}

void CDlgEthernet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_DEST, _remoteIpAddr);
}


BEGIN_MESSAGE_MAP(CDlgEthernet, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgEthernet::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_BROADCAST, &CDlgEthernet::OnBnClickedCheckBroadcast)
	ON_BN_CLICKED(IDCANCEL, &CDlgEthernet::OnBnClickedCancel)
END_MESSAGE_MAP()


void CDlgEthernet::SetConnectionMode (int mode)
{
	GetDlgItem (IDC_COMBO_LOCAL_IP) ->EnableWindow ( (mode != 2) ? TRUE : FALSE);	// 0-UDP, 1-TCP Server에서 활성화
	GetDlgItem (IDC_IPADDRESS_DEST) ->EnableWindow ( (mode != 1) ? TRUE : FALSE);	// 0-UDP, 2-TCP Client에서 활성화
	GetDlgItem (IDC_CHECK_BROADCAST)->EnableWindow ( (mode == 0) ? TRUE : FALSE);	// 0-UDP에서 활성화
	GetDlgItem (IDC_EDIT_DEST_PORT) ->EnableWindow (TRUE);
}

BOOL CDlgEthernet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 
	_remoteIpAddr.SetAddress (g_ini.remoteIpAddr);
	SetDlgItemInt (IDC_EDIT_DEST_PORT, g_ini.portNum, FALSE);

	vector<DWORD> host_ip = GetHostIPAddress();

	SendDlgItemMessage (IDC_COMBO_LOCAL_IP, CB_INSERTSTRING, 0, (LPARAM)"0.0.0.0 (Any)");			// INADDR_ANY
	SendDlgItemMessage (IDC_COMBO_LOCAL_IP, CB_INSERTSTRING, 1, (LPARAM)"127.0.0.1 (Loopback)");	// INADDR_LOOPBACK

	int sel = 0;
	if (g_ini.localIpAddr == INADDR_ANY)      sel = 0;
	if (g_ini.localIpAddr == INADDR_LOOPBACK) sel = 1;

	int count = 2;
	for (int i=0; i<(int)host_ip.size(); i++) {
		DWORD ip = ntohl(host_ip[i]);
		if (ip != INADDR_ANY && ip != INADDR_LOOPBACK) { 
			SendDlgItemMessage (IDC_COMBO_LOCAL_IP, CB_INSERTSTRING, count, (LPARAM)ip_addr_str(ip));
			if (g_ini.localIpAddr == ip) sel = count;
			count++;
		}
	}
	SendDlgItemMessage (IDC_COMBO_LOCAL_IP, CB_SETCURSEL, sel);

	CheckDlgButton (IDC_CHECK_BROADCAST, (g_ini.remoteIpAddr == INADDR_BROADCAST) ? BST_CHECKED : BST_UNCHECKED);
	OnBnClickedCheckBroadcast();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEthernet::OnBnClickedOk()
{
	CString buff;
	GetDlgItemText (IDC_COMBO_LOCAL_IP, buff);
	
	g_ini.localIpAddr = htonl (inet_addr ((LPCSTR)buff));
	_remoteIpAddr.GetAddress ((DWORD &)g_ini.remoteIpAddr);
	g_ini.portNum = GetDlgItemInt (IDC_EDIT_DEST_PORT, FALSE);

	OnOK();
}

void CDlgEthernet::OnBnClickedCheckBroadcast()
{
	if (IsDlgButtonChecked (IDC_CHECK_BROADCAST) == BST_CHECKED) {
		_remoteIpAddr.SetAddress (INADDR_BROADCAST);
		_remoteIpAddr.EnableWindow (FALSE);
	}
	else {
		_remoteIpAddr.EnableWindow (TRUE);
	}

}

void CDlgEthernet::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//OnCancel();
}

#include "stdafx.h"
#include "SerialPortMon.h"
#include "DlgUdp.h"
#include "Udp.h"
#include "SockCommon.h"

extern CCommPort *g_comm;


IMPLEMENT_DYNAMIC(CDlgUdp, CDialog)

CDlgUdp::CDlgUdp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUdp::IDD, pParent)
{
}

CDlgUdp::~CDlgUdp()
{
}

void CDlgUdp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgUdp, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlgUdp message handlers


BOOL CDlgUdp::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText (IDC_EDIT_IP,   "");
	SetDlgItemText (IDC_EDIT_PORT, "");

	SetTimer (1000, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgUdp::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		if (IsWindowVisible ()) {
			CUdp *udp = dynamic_cast<CUdp *>(g_comm);
			if (udp) {
				bool connect = (g_comm) ? g_comm->IsConnected () : false;
				if (connect && udp->_recvIpAddr && udp->_recvPort) {
					SetDlgItemText (IDC_EDIT_IP,   ip_addr_str (udp->_recvIpAddr));
					SetDlgItemInt  (IDC_EDIT_PORT, udp->_recvPort);
				}
				else {
					SetDlgItemText (IDC_EDIT_IP,   "");
					SetDlgItemText (IDC_EDIT_PORT, "");
				}
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgUdp::OnDestroy()
{
	CDialog::OnDestroy();

	KillTimer (1000);
}
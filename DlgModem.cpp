#include "stdafx.h"
#include "SerialPortMon.h"
#include "DlgModem.h"
#include "SerialCOM.h"

extern CCommPort *g_comm;

IMPLEMENT_DYNAMIC(CDlgModem, CDialog)

CDlgModem::CDlgModem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModem::IDD, pParent)
{
}

CDlgModem::~CDlgModem()
{
}

void CDlgModem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgModem, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_BREAK, &CDlgModem::OnBnClickedCheckBreak)
	ON_BN_CLICKED(IDC_CHECK_RTS, &CDlgModem::OnBnClickedCheckRts)
	ON_BN_CLICKED(IDC_CHECK_DTR, &CDlgModem::OnBnClickedCheckDtr)
	ON_BN_CLICKED(IDC_BUTTON_XON, &CDlgModem::OnBnClickedButtonXon)
	ON_BN_CLICKED(IDC_BUTTON_XOFF, &CDlgModem::OnBnClickedButtonXoff)
END_MESSAGE_MAP()


BOOL CDlgModem::OnInitDialog()
{
	CDialog::OnInitDialog();

	HINSTANCE hInst = AfxGetInstanceHandle();

	COLORMAP colorMap;
	colorMap.from = RGB(0xFF,0xFF,0xFF);
	colorMap.to  = ::GetSysColor( COLOR_BTNFACE );

	HB_LAMP_DIS		= ::CreateMappedBitmap(hInst, IDB_LAMP_DISABLE, 0, &colorMap, 1);
	HB_LAMP_OFF		= ::CreateMappedBitmap(hInst, IDB_LAMP_OFF,		0, &colorMap, 1);
	HB_LAMP_ON		= ::CreateMappedBitmap(hInst, IDB_LAMP_ON,		0, &colorMap, 1); 

	CheckDlgButton (IDC_CHECK_BREAK, BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_RTS,   BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_DTR,   BST_UNCHECKED);

	SetTimer (1000, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CDlgModem::IsObjDeleteable(HGDIOBJ hGdiObj)
{
	if(	(hGdiObj ==  HB_LAMP_DIS) ||
		(hGdiObj ==  HB_LAMP_OFF) ||
		(hGdiObj ==  HB_LAMP_ON) ){
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgModem::ReplaceStaticControlBitmap (int ControlID, HBITMAP hBitmap)
{
	HGDIOBJ hOldObject = (HGDIOBJ)SendDlgItemMessage (ControlID, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
	if (hOldObject) {
		if (IsObjDeleteable(hOldObject)) {
			::DeleteObject (hOldObject);
		}
		return TRUE;
	}
	return FALSE;
}

HBITMAP CDlgModem::SelectBitmap (bool connected, bool flag)
{
	if (!connected) return HB_LAMP_DIS;
	else if (!flag) return HB_LAMP_OFF;
	else            return HB_LAMP_ON;
}

void CDlgModem::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		if (IsWindowVisible ()) {
			DWORD modemStat = 0;
			CSerialCOM *com = dynamic_cast<CSerialCOM *>(g_comm);
			if (com) com->GetCommModemStat (modemStat);
			bool connect = (g_comm) ? g_comm->IsConnected () : false;
			
			static HBITMAP cts_prv = 0, dsr_prv = 0, ring_prv = 0, rlsd_prv = 0;
			HBITMAP cts  = SelectBitmap (connect, !!(modemStat & MS_CTS_ON ));
			HBITMAP dsr  = SelectBitmap (connect, !!(modemStat & MS_DSR_ON ));
			HBITMAP ring = SelectBitmap (connect, !!(modemStat & MS_RING_ON));
			HBITMAP rlsd = SelectBitmap (connect, !!(modemStat & MS_RLSD_ON));

			if (cts_prv != cts) {
				ReplaceStaticControlBitmap (IDC_STATIC_CTS , cts); 
				cts_prv = cts;
			}
			if (dsr_prv != dsr) {
				ReplaceStaticControlBitmap (IDC_STATIC_DSR , dsr); 
				dsr_prv = dsr;
			}
			if (ring_prv != ring) {
				ReplaceStaticControlBitmap (IDC_STATIC_RING, ring); 
				ring_prv = ring;
			}
			if (rlsd_prv != rlsd) {
				ReplaceStaticControlBitmap (IDC_STATIC_RLSD, rlsd); 
				rlsd_prv = rlsd;
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgModem::OnDestroy()
{
	CDialog::OnDestroy();

	KillTimer (1000);

	DeleteObject (HB_LAMP_DIS);
	DeleteObject (HB_LAMP_OFF);
	DeleteObject (HB_LAMP_ON);
}

/*
	COM Port D-sub 커넥터 pin 정보:
	1 CD (Carier Dectect)		데이타 채널 수신 캐리어 검출
	2 TD (Transmit Data)		송신 보내기 = TX
	3 RD (Receive Data)			수신 받기 = RX
	4 DTR(Data Terminal Ready)	터미날 장치 준비
	5 SG (Signal Group)			신호용 접지 = GRD
	6 DSR(Data Set Ready)		수신 준비 완료
	7 RTS(Request To Send)		송신 요구
	8 CTS(Clear To Send)		송신 가능 
	9 RI (Ring Indicator)		피 호출 표시
*/

void CDlgModem::OnBnClickedCheckBreak()
{
	CSerialCOM *com = dynamic_cast<CSerialCOM *>(g_comm);
	if (com) com->EscapeCommFunc ((IsDlgButtonChecked (IDC_CHECK_BREAK) == BST_CHECKED) ? SETBREAK : CLRBREAK);
}

void CDlgModem::OnBnClickedCheckRts()
{
	CSerialCOM *com = dynamic_cast<CSerialCOM *>(g_comm);
	if (com) com->EscapeCommFunc ((IsDlgButtonChecked (IDC_CHECK_RTS) == BST_CHECKED) ? SETRTS : CLRRTS);
}

void CDlgModem::OnBnClickedCheckDtr()
{
	CSerialCOM *com = dynamic_cast<CSerialCOM *>(g_comm);
	if (com) com->EscapeCommFunc ((IsDlgButtonChecked (IDC_CHECK_DTR) == BST_CHECKED) ? SETDTR : CLRDTR);
}

void CDlgModem::OnBnClickedButtonXon()
{
	CSerialCOM *com = dynamic_cast<CSerialCOM *>(g_comm);
	if (com) com->EscapeCommFunc (SETXON);
}

void CDlgModem::OnBnClickedButtonXoff()
{
	CSerialCOM *com = dynamic_cast<CSerialCOM *>(g_comm);
	if (com) com->EscapeCommFunc (SETXON);
}

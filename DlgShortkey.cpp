#include "stdafx.h"
#include "SerialPortMon.h"
#include "DlgShortkey.h"
#include "AppIni.h"
#include "Common.h"
#include "escape_seq.h"
#include "virtual_key.h"
#include "shortkey.h"
#include "CommPort.h"


int IDC_EDIT_MESSAGE[8]		= { IDC_EDIT_MESSAGE1,		IDC_EDIT_MESSAGE2,		IDC_EDIT_MESSAGE3,		IDC_EDIT_MESSAGE4,		IDC_EDIT_MESSAGE5,		IDC_EDIT_MESSAGE6,		IDC_EDIT_MESSAGE7,		IDC_EDIT_MESSAGE8,		};
int IDC_CHECK_ASCII_HEX[8]	= { IDC_CHECK_ASCII_HEX1,	IDC_CHECK_ASCII_HEX2,	IDC_CHECK_ASCII_HEX3,	IDC_CHECK_ASCII_HEX4,	IDC_CHECK_ASCII_HEX5,	IDC_CHECK_ASCII_HEX6,	IDC_CHECK_ASCII_HEX7,	IDC_CHECK_ASCII_HEX8,	};
int IDC_EDIT_DELAY[8]		= { IDC_EDIT_DELAY1,		IDC_EDIT_DELAY2,		IDC_EDIT_DELAY3,		IDC_EDIT_DELAY4,		IDC_EDIT_DELAY5,		IDC_EDIT_DELAY6,		IDC_EDIT_DELAY7,		IDC_EDIT_DELAY8,		};
int IDC_EDIT_REPEAT[8]		= { IDC_EDIT_REPEAT1,		IDC_EDIT_REPEAT2,		IDC_EDIT_REPEAT3,		IDC_EDIT_REPEAT4,		IDC_EDIT_REPEAT5,		IDC_EDIT_REPEAT6,		IDC_EDIT_REPEAT7,		IDC_EDIT_REPEAT8,		};
int IDC_STATIC_RUN[8]		= { IDC_STATIC_RUN1,		IDC_STATIC_RUN2,		IDC_STATIC_RUN3,		IDC_STATIC_RUN4,		IDC_STATIC_RUN5,		IDC_STATIC_RUN6,		IDC_STATIC_RUN7,		IDC_STATIC_RUN8,		};
int IDC_COMBO_SHORTKEY[9]	= { IDC_COMBO_SHORTKEY1,	IDC_COMBO_SHORTKEY2,	IDC_COMBO_SHORTKEY3,	IDC_COMBO_SHORTKEY4,	IDC_COMBO_SHORTKEY5,	IDC_COMBO_SHORTKEY6,	IDC_COMBO_SHORTKEY7,	IDC_COMBO_SHORTKEY8,	IDC_COMBO_SHORTKEY9,	};
int IDC_CHECK_CTRL[9]		= { IDC_CHECK_CTRL1,		IDC_CHECK_CTRL2,		IDC_CHECK_CTRL3,		IDC_CHECK_CTRL4,		IDC_CHECK_CTRL5,		IDC_CHECK_CTRL6,		IDC_CHECK_CTRL7,		IDC_CHECK_CTRL8,		IDC_CHECK_CTRL9,		};
int IDC_CHECK_ALT[9]		= { IDC_CHECK_ALT1,			IDC_CHECK_ALT2,			IDC_CHECK_ALT3,			IDC_CHECK_ALT4,			IDC_CHECK_ALT5,			IDC_CHECK_ALT6,			IDC_CHECK_ALT7,			IDC_CHECK_ALT8,			IDC_CHECK_ALT9,			};
int IDC_CHECK_SHIFT[9]		= { IDC_CHECK_SHIFT1,		IDC_CHECK_SHIFT2,		IDC_CHECK_SHIFT3,		IDC_CHECK_SHIFT4,		IDC_CHECK_SHIFT5,		IDC_CHECK_SHIFT6,		IDC_CHECK_SHIFT7,		IDC_CHECK_SHIFT8,		IDC_CHECK_SHIFT9,		};


sShortkey _shortkey[8];
CDlgShortkey *g_dlgShortkey = NULL;
extern CCommPort *g_comm;


IMPLEMENT_DYNAMIC(CDlgShortkey, CDialog)


CDlgShortkey::CDlgShortkey(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgShortkey::IDD, pParent)
{
	_enable_prev = -1;
	g_dlgShortkey = this;
}

CDlgShortkey::~CDlgShortkey()
{
	g_dlgShortkey = NULL;
}

void CDlgShortkey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgShortkey, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_INPUT()
	ON_BN_CLICKED(IDOK, &CDlgShortkey::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgShortkey::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_HIDE_SHORTKEY, &CDlgShortkey::OnBnClickedCheckHideShortkey)
	ON_BN_CLICKED(IDC_BUTTON_SEND1, &CDlgShortkey::OnBnClickedButtonSend1)
	ON_BN_CLICKED(IDC_BUTTON_SEND2, &CDlgShortkey::OnBnClickedButtonSend2)
	ON_BN_CLICKED(IDC_BUTTON_SEND3, &CDlgShortkey::OnBnClickedButtonSend3)
	ON_BN_CLICKED(IDC_BUTTON_SEND4, &CDlgShortkey::OnBnClickedButtonSend4)
	ON_BN_CLICKED(IDC_BUTTON_SEND5, &CDlgShortkey::OnBnClickedButtonSend5)
	ON_BN_CLICKED(IDC_BUTTON_SEND6, &CDlgShortkey::OnBnClickedButtonSend6)
	ON_BN_CLICKED(IDC_BUTTON_SEND7, &CDlgShortkey::OnBnClickedButtonSend7)
	ON_BN_CLICKED(IDC_BUTTON_SEND8, &CDlgShortkey::OnBnClickedButtonSend8)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDlgShortkey::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_CHECK_ASCII_HEX1, &CDlgShortkey::OnBnClickedCheckAsciiHex1)
	ON_BN_CLICKED(IDC_CHECK_ASCII_HEX2, &CDlgShortkey::OnBnClickedCheckAsciiHex2)
	ON_BN_CLICKED(IDC_CHECK_ASCII_HEX3, &CDlgShortkey::OnBnClickedCheckAsciiHex3)
	ON_BN_CLICKED(IDC_CHECK_ASCII_HEX4, &CDlgShortkey::OnBnClickedCheckAsciiHex4)
	ON_BN_CLICKED(IDC_CHECK_ASCII_HEX5, &CDlgShortkey::OnBnClickedCheckAsciiHex5)
	ON_BN_CLICKED(IDC_CHECK_ASCII_HEX6, &CDlgShortkey::OnBnClickedCheckAsciiHex6)
	ON_BN_CLICKED(IDC_CHECK_ASCII_HEX7, &CDlgShortkey::OnBnClickedCheckAsciiHex7)
	ON_BN_CLICKED(IDC_CHECK_ASCII_HEX8, &CDlgShortkey::OnBnClickedCheckAsciiHex8)
	ON_CBN_SELCHANGE(IDC_COMBO_SHORTKEY1, &CDlgShortkey::OnCbnSelchangeComboShortkey1)
	ON_CBN_SELCHANGE(IDC_COMBO_SHORTKEY2, &CDlgShortkey::OnCbnSelchangeComboShortkey2)
	ON_CBN_SELCHANGE(IDC_COMBO_SHORTKEY3, &CDlgShortkey::OnCbnSelchangeComboShortkey3)
	ON_CBN_SELCHANGE(IDC_COMBO_SHORTKEY4, &CDlgShortkey::OnCbnSelchangeComboShortkey4)
	ON_CBN_SELCHANGE(IDC_COMBO_SHORTKEY5, &CDlgShortkey::OnCbnSelchangeComboShortkey5)
	ON_CBN_SELCHANGE(IDC_COMBO_SHORTKEY6, &CDlgShortkey::OnCbnSelchangeComboShortkey6)
	ON_CBN_SELCHANGE(IDC_COMBO_SHORTKEY7, &CDlgShortkey::OnCbnSelchangeComboShortkey7)
	ON_CBN_SELCHANGE(IDC_COMBO_SHORTKEY8, &CDlgShortkey::OnCbnSelchangeComboShortkey8)
	ON_CBN_SELCHANGE(IDC_COMBO_SHORTKEY9, &CDlgShortkey::OnCbnSelchangeComboShortkey9)
	ON_BN_CLICKED(IDC_CHECK_CTRL1, &CDlgShortkey::OnBnClickedCheckCtrl1)
	ON_BN_CLICKED(IDC_CHECK_CTRL2, &CDlgShortkey::OnBnClickedCheckCtrl2)
	ON_BN_CLICKED(IDC_CHECK_CTRL3, &CDlgShortkey::OnBnClickedCheckCtrl3)
	ON_BN_CLICKED(IDC_CHECK_CTRL4, &CDlgShortkey::OnBnClickedCheckCtrl4)
	ON_BN_CLICKED(IDC_CHECK_CTRL5, &CDlgShortkey::OnBnClickedCheckCtrl5)
	ON_BN_CLICKED(IDC_CHECK_CTRL6, &CDlgShortkey::OnBnClickedCheckCtrl6)
	ON_BN_CLICKED(IDC_CHECK_CTRL7, &CDlgShortkey::OnBnClickedCheckCtrl7)
	ON_BN_CLICKED(IDC_CHECK_CTRL8, &CDlgShortkey::OnBnClickedCheckCtrl8)
	ON_BN_CLICKED(IDC_CHECK_CTRL9, &CDlgShortkey::OnBnClickedCheckCtrl9)
	ON_BN_CLICKED(IDC_CHECK_ALT1, &CDlgShortkey::OnBnClickedCheckAlt1)
	ON_BN_CLICKED(IDC_CHECK_ALT2, &CDlgShortkey::OnBnClickedCheckAlt2)
	ON_BN_CLICKED(IDC_CHECK_ALT3, &CDlgShortkey::OnBnClickedCheckAlt3)
	ON_BN_CLICKED(IDC_CHECK_ALT4, &CDlgShortkey::OnBnClickedCheckAlt4)
	ON_BN_CLICKED(IDC_CHECK_ALT5, &CDlgShortkey::OnBnClickedCheckAlt5)
	ON_BN_CLICKED(IDC_CHECK_ALT6, &CDlgShortkey::OnBnClickedCheckAlt6)
	ON_BN_CLICKED(IDC_CHECK_ALT7, &CDlgShortkey::OnBnClickedCheckAlt7)
	ON_BN_CLICKED(IDC_CHECK_ALT8, &CDlgShortkey::OnBnClickedCheckAlt8)
	ON_BN_CLICKED(IDC_CHECK_ALT9, &CDlgShortkey::OnBnClickedCheckAlt9)
	ON_BN_CLICKED(IDC_CHECK_SHIFT1, &CDlgShortkey::OnBnClickedCheckShift1)
	ON_BN_CLICKED(IDC_CHECK_SHIFT2, &CDlgShortkey::OnBnClickedCheckShift2)
	ON_BN_CLICKED(IDC_CHECK_SHIFT3, &CDlgShortkey::OnBnClickedCheckShift3)
	ON_BN_CLICKED(IDC_CHECK_SHIFT4, &CDlgShortkey::OnBnClickedCheckShift4)
	ON_BN_CLICKED(IDC_CHECK_SHIFT5, &CDlgShortkey::OnBnClickedCheckShift5)
	ON_BN_CLICKED(IDC_CHECK_SHIFT6, &CDlgShortkey::OnBnClickedCheckShift6)
	ON_BN_CLICKED(IDC_CHECK_SHIFT7, &CDlgShortkey::OnBnClickedCheckShift7)
	ON_BN_CLICKED(IDC_CHECK_SHIFT8, &CDlgShortkey::OnBnClickedCheckShift8)
	ON_BN_CLICKED(IDC_CHECK_SHIFT9, &CDlgShortkey::OnBnClickedCheckShift9)
END_MESSAGE_MAP()


void CDlgShortkey::DlgInScreen ()
{
	RECT rect;
	GetWindowRect (&rect);

	int cxscreen = GetSystemMetrics(SM_CXVIRTUALSCREEN) - (rect.right - rect.left) - 1;
	int cyscreen = GetSystemMetrics(SM_CYVIRTUALSCREEN) - (rect.bottom - rect.top) - 1;

	if (g_ini.mainWndSx < 0) g_ini.mainWndSx = 0;
	if (g_ini.mainWndSx > cxscreen) g_ini.mainWndSx = cxscreen;

	if (g_ini.mainWndSy < 0) g_ini.mainWndSy = 0;
	if (g_ini.mainWndSy > cyscreen) g_ini.mainWndSy = cyscreen;
}


BOOL CDlgShortkey::OnInitDialog()
{
	CDialog::OnInitDialog();

	const int RID_size = 1;
	RAWINPUTDEVICE rid[RID_size];
	ZeroMemory (rid, sizeof(RAWINPUTDEVICE)*RID_size);

	// 키보드
	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x06;
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = m_hWnd;

	// 마우스
	//rid[1].usUsagePage = 0x01;
	//rid[1].usUsage = 0x02;
	//rid[1].dwFlags = RIDEV_INPUTSINK;
	//rid[1].hwndTarget = m_hWnd;    //    메시지를 처리할 프로시져의 윈도우 핸들

	if (!RegisterRawInputDevices (rid, RID_size, sizeof(RAWINPUTDEVICE))) {
		TRACE ("RegisterRawInputDevices Failed");
	}

	// 매크로 테스트 숨기기/보이기 선택 옵션에 따라 대화상자 크기를 조절하기위해
	// 대화상자의 가로, 세로 크기를 저장해 둔다.
	RECT rect;
	GetWindowRect (&rect);
	_dlg_cx = rect.right - rect.left;
	_dlg_cy = rect.bottom - rect.top;

	CheckDlgButton (IDC_CHECK_HIDE_SHORTKEY, (g_ini.options.HIDE_SHORTKEY_PART) ? BST_CHECKED : BST_UNCHECKED);
	OnBnClickedCheckHideShortkey ();

	//
	DlgInScreen ();
	SetWindowPos (NULL, g_ini.shortkeyWndSx, g_ini.shortkeyWndSy, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	HINSTANCE hInst = AfxGetInstanceHandle();

	COLORMAP colorMap;
	colorMap.from = RGB(0xFF,0xFF,0xFF);
	colorMap.to  = ::GetSysColor( COLOR_BTNFACE );

	HB_LAMP_DIS		= ::CreateMappedBitmap(hInst, IDB_LAMP_DISABLE, 0, &colorMap, 1);
	HB_LAMP_OFF		= ::CreateMappedBitmap(hInst, IDB_LAMP_OFF,		0, &colorMap, 1);
	HB_LAMP_ON		= ::CreateMappedBitmap(hInst, IDB_LAMP_ON,		0, &colorMap, 1); 

	for (int i=0; i<9; i++) {
		if (i < 8) {
			SetDlgItemText (IDC_EDIT_MESSAGE[i], g_ini.message[i]);
			CheckDlgButton (IDC_CHECK_ASCII_HEX[i], (g_ini.msgOpt[i].SHORTKEY_DISPLAY_HEX) ? BST_CHECKED : BST_UNCHECKED);
			SetDlgItemInt  (IDC_EDIT_DELAY[i],  BOUND(g_ini.delay[i], 10, 60000), FALSE);
			SetDlgItemInt  (IDC_EDIT_REPEAT[i], BOUND(g_ini.repeat[i], 1,  1000), FALSE);
			_message_hexa[i] = false;
		}
		SendDlgItemMessage (IDC_COMBO_SHORTKEY[i], CB_INITSTORAGE, 256, 100);
		SendDlgItemMessage (IDC_COMBO_SHORTKEY[i], WM_SETREDRAW, 0, 0);
		for (int j=0; j<256 && g_vk_list[j].vk >= 0; ++j) {
			SendDlgItemMessage (IDC_COMBO_SHORTKEY[i], CB_INSERTSTRING, j, (LPARAM)g_vk_list[j].desc);
		}
		SendDlgItemMessage (IDC_COMBO_SHORTKEY[i],  CB_SETCURSEL, getVkIndex(g_ini.shortKey[i].vk));
		SendDlgItemMessage (IDC_COMBO_SHORTKEY[i], WM_SETREDRAW, 1, 0);
	
		CheckDlgButton (IDC_CHECK_CTRL[i],	(g_ini.shortKey[i].VK_KEY_CTRL ) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton (IDC_CHECK_ALT[i],	(g_ini.shortKey[i].VK_KEY_ALT  ) ? BST_CHECKED : BST_UNCHECKED);	
		CheckDlgButton (IDC_CHECK_SHIFT[i],	(g_ini.shortKey[i].VK_KEY_SHIFT) ? BST_CHECKED : BST_UNCHECKED);	
	}

	OnBnClickedCheckAsciiHex1();
	OnBnClickedCheckAsciiHex2();
	OnBnClickedCheckAsciiHex3();
	OnBnClickedCheckAsciiHex4();
	OnBnClickedCheckAsciiHex5();
	OnBnClickedCheckAsciiHex6();
	OnBnClickedCheckAsciiHex7();
	OnBnClickedCheckAsciiHex8();

	SetTimer (1000, 10, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgShortkey::IsObjDeleteable(HGDIOBJ hGdiObj)
{
	if(	(hGdiObj ==  HB_LAMP_DIS) ||
		(hGdiObj ==  HB_LAMP_OFF) ||
		(hGdiObj ==  HB_LAMP_ON) ){
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgShortkey::ReplaceStaticControlBitmap (int ControlID, HBITMAP hBitmap)
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

HBITMAP CDlgShortkey::SelectBitmap (bool connected, bool flag)
{
	if (!connected) return HB_LAMP_DIS;
	else if (!flag) return HB_LAMP_OFF;
	else            return HB_LAMP_ON;
}


void CDlgShortkey::OnTimer(UINT_PTR nIDEvent)
{
	DWORD t = GetTickCount ();

	if (nIDEvent == 1000) {
		for (int i=0; i<8; i++) {
			_shortkey[i].Step (t);
		}

		bool connect = (g_comm) ? g_comm->IsConnected () : false;
		int enable = (connect) ? 1 : 0;
		
		if (_enable_prev != enable) {
			EnableWindowItem (enable);
			
			if (!enable) OnBnClickedButtonStop();

			_enable_prev = enable;
		}

		static HBITMAP bm_prev[8] = { HB_LAMP_DIS, HB_LAMP_DIS, HB_LAMP_DIS, HB_LAMP_DIS, HB_LAMP_DIS, HB_LAMP_DIS, HB_LAMP_DIS, HB_LAMP_DIS, };
		
		for (int i=0; i<8; i++) {
			HBITMAP bm = SelectBitmap (connect, _shortkey[i].IsRun ());
			if (bm_prev[i] != bm) {
				ReplaceStaticControlBitmap (IDC_STATIC_RUN[i], bm);
				bm_prev[i] = bm;
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgShortkey::OnDestroy()
{
	KillTimer (1000);

	DeleteObject (HB_LAMP_DIS);
	DeleteObject (HB_LAMP_OFF);
	DeleteObject (HB_LAMP_ON);

	RECT rectWnd;
	GetWindowRect (&rectWnd);

	g_ini.shortkeyWndSx = rectWnd.left;
	g_ini.shortkeyWndSy = rectWnd.top;

	for (int i=0; i<9; i++) {
		if (i < 8) {
			if (_message_hexa[i]) {
				ChangeDlgItemTextFormat (IDC_EDIT_MESSAGE[i], false);
			}
			GetDlgItemText (IDC_EDIT_MESSAGE[i], g_ini.message[i], 256);
			g_ini.message[i][256] = '\0';

			g_ini.msgOpt[i].SHORTKEY_DISPLAY_HEX = IsDlgButtonChecked (IDC_CHECK_ASCII_HEX[i]) == BST_CHECKED;

			g_ini.delay[i]  = BOUND (GetDlgItemInt  (IDC_EDIT_DELAY[i],  FALSE), 10, 60000);
			g_ini.repeat[i] = BOUND (GetDlgItemInt  (IDC_EDIT_REPEAT[i], FALSE),  1,  1000);
		}		
		int index = SendDlgItemMessage (IDC_COMBO_SHORTKEY[i],  CB_GETCURSEL);
		g_ini.shortKey[i].vk = GetVirtualKeyCode (index);

		g_ini.shortKey[i].VK_KEY_CTRL = IsDlgButtonChecked (IDC_CHECK_CTRL[i])  == BST_CHECKED;
		g_ini.shortKey[i].VK_KEY_ALT  = IsDlgButtonChecked (IDC_CHECK_ALT[i])   == BST_CHECKED;
		g_ini.shortKey[i].VK_KEY_SHIFT= IsDlgButtonChecked (IDC_CHECK_SHIFT[i]) == BST_CHECKED;
	}

	CDialog::OnDestroy();
}


void CDlgShortkey::OnRawInput(UINT nInputcode, HRAWINPUT hRawInput)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.

	RAWINPUT rawInput;
    ZeroMemory (&rawInput, sizeof(RAWINPUT));
    
	UINT unSize = sizeof(RAWINPUT);
    ::GetRawInputData ((HRAWINPUT)hRawInput, RID_INPUT, &rawInput, &unSize, sizeof(RAWINPUTHEADER));

    switch (rawInput.header.dwType) {
    case RIM_TYPEKEYBOARD:
		if( rawInput.data.keyboard.Message == WM_KEYDOWN || rawInput.data.keyboard.Message == WM_SYSKEYDOWN ) {
			sVirtualKey vk = { rawInput.data.keyboard.VKey, 0, 0, 0, 0 };
			OnKeyEvent (vk);
		}
        break;
    case RIM_TYPEMOUSE:
		// 사용 안하고 있음
        break;
    case RIM_TYPEHID:
        break;
    default:
        break;
    }

	CDialog::OnRawInput(nInputcode, hRawInput);
}

void CDlgShortkey::OnBnClickedOk()
{
	ShowWindow (SW_HIDE);
	// OnOK();
}

void CDlgShortkey::OnBnClickedCancel()
{
	ShowWindow (SW_HIDE);
	// OnCancel();
}

void CDlgShortkey::EnableWindowItem (BOOL enable)
{
	GetDlgItem (IDC_BUTTON_SEND1)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_SEND2)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_SEND3)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_SEND4)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_SEND5)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_SEND6)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_SEND7)->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_SEND8)->EnableWindow (enable);
}

void CDlgShortkey::OnKeyEvent (sVirtualKey vk)
{
	if (GetAsyncKeyState (VK_LSHIFT))	vk.VK_KEY_SHIFT = 1;
	if (GetAsyncKeyState (VK_RSHIFT))	vk.VK_KEY_SHIFT = 1;
	if (GetAsyncKeyState (VK_LCONTROL))	vk.VK_KEY_CTRL = 1;
	if (GetAsyncKeyState (VK_RCONTROL))	vk.VK_KEY_CTRL = 1;
	if (GetAsyncKeyState (VK_LMENU))	vk.VK_KEY_ALT = 1;
	if (GetAsyncKeyState (VK_RMENU))	vk.VK_KEY_ALT = 1;

	for (int i=0; i<8; i++) {
		if ((DWORD &)g_ini.shortKey[i] == (DWORD &)vk) {
			OnBnClickedButtonSend (i);
		}
	}
	int i = 8;
	if ((DWORD &)g_ini.shortKey[i] == (DWORD &)vk) {
		OnBnClickedButtonStop ();
	}

	SetDlgItemText (IDC_EDIT_KEY_PRESSED, GetVirtualKeyDescFromVK (vk.vk));
}

void CDlgShortkey::OnBnClickedCheckHideShortkey()
{
	g_ini.options.HIDE_SHORTKEY_PART = IsDlgButtonChecked (IDC_CHECK_HIDE_SHORTKEY) == BST_CHECKED;
	int dx = (g_ini.options.HIDE_SHORTKEY_PART) ? -250 : 0;

	SetWindowPos (NULL, 0, 0, _dlg_cx + dx, _dlg_cy, SWP_NOMOVE | SWP_NOZORDER);
}

void CDlgShortkey::OnBnClickedButtonSend (int i)
{
	if (g_comm && g_comm->IsConnected ());
	else return;

	// 현재 실행 중이라면 실행을 중지한다.
	if (_shortkey[i].IsRun ()) {
		_shortkey[i].Stop ();
		return;
	}

	// 실행 중이 아니라면 실행을 시작한다.
	CString message;
	GetDlgItemText (IDC_EDIT_MESSAGE[i], message);

	int len = message.GetLength ();
	if (!len) return;

	char *str = new char [len + 10];

	if (_message_hexa[i]) {
		len = hexachar2str ((LPCSTR)message, str, len);
	}
	else {
		len = escseq2str ((LPCSTR)message, str, len);
	}

	g_ini.delay[i]  = GetDlgItemInt  (IDC_EDIT_DELAY[i],  FALSE);
	g_ini.repeat[i] = GetDlgItemInt  (IDC_EDIT_REPEAT[i], FALSE);

	if (g_ini.delay[i]  < 1) g_ini.delay[i] = 1;
	if (g_ini.repeat[i] < 1) g_ini.repeat[i] = 1;

	_shortkey[i].Start (str, len, g_ini.delay[i], g_ini.repeat[i]);
	
	delete [] str;
}

void CDlgShortkey::OnBnClickedButtonStop()
{
	for (int i=0; i<8; i++) {
		_shortkey[i].Stop ();
	}
}


void CDlgShortkey::ChangeDlgItemTextFormat (int nIDEdit, bool hexa)
{
	CString buff;
	GetDlgItemText (nIDEdit, buff);

	if (hexa) {
		char *hexachar = new char [3*buff.GetLength () + 10];
		int n = escseq2hexachar ((LPCSTR)buff, hexachar, 3*buff.GetLength () + 9);
		hexachar[n] = '\0';

		SetDlgItemText (nIDEdit, hexachar);
		delete [] hexachar;
	}
	else {
		char *escseq = new char [buff.GetLength () + 10];
		int n = hexachar2escseq ((LPCSTR)buff, escseq, buff.GetLength () + 9);
		escseq[n] = '\0';

		SetDlgItemText (nIDEdit, escseq);
		delete [] escseq;
	}
}

void CDlgShortkey::OnBnClickedCheckAsciiHex(int i)
{
	g_ini.msgOpt[i].SHORTKEY_DISPLAY_HEX = IsDlgButtonChecked (IDC_CHECK_ASCII_HEX[i]) == BST_CHECKED;
	SetDlgItemText (IDC_CHECK_ASCII_HEX[i], g_ini.msgOpt[i].SHORTKEY_DISPLAY_HEX ? "HEX" : "ASCII");

	bool message_hexa = g_ini.msgOpt[i].SHORTKEY_DISPLAY_HEX ? true : false;
	if (_message_hexa[i] != message_hexa) {
		ChangeDlgItemTextFormat (IDC_EDIT_MESSAGE[i], message_hexa);
		_message_hexa[i] = message_hexa;
	}
}

void CDlgShortkey::OnCbnSelchangeComboShortkey(int i) 
{ 
	int index = SendDlgItemMessage (IDC_COMBO_SHORTKEY[i],  CB_GETCURSEL);
	g_ini.shortKey[i].vk = GetVirtualKeyCode (index);
}

void CDlgShortkey::OnBnClickedCheckCtrl(int i) 
{ 
	g_ini.shortKey[i].VK_KEY_CTRL = IsDlgButtonChecked (IDC_CHECK_CTRL[i])  == BST_CHECKED;
}

void CDlgShortkey::OnBnClickedCheckAlt(int i)  
{ 
	g_ini.shortKey[i].VK_KEY_ALT = IsDlgButtonChecked (IDC_CHECK_ALT[i])   == BST_CHECKED;
}

void CDlgShortkey::OnBnClickedCheckShift(int i) 
{ 
	g_ini.shortKey[i].VK_KEY_SHIFT = IsDlgButtonChecked (IDC_CHECK_SHIFT[i]) == BST_CHECKED;
}

void CDlgShortkey::OnBnClickedButtonSend1() { OnBnClickedButtonSend (0); }
void CDlgShortkey::OnBnClickedButtonSend2() { OnBnClickedButtonSend (1); }
void CDlgShortkey::OnBnClickedButtonSend3() { OnBnClickedButtonSend (2); }
void CDlgShortkey::OnBnClickedButtonSend4() { OnBnClickedButtonSend (3); }
void CDlgShortkey::OnBnClickedButtonSend5() { OnBnClickedButtonSend (4); }
void CDlgShortkey::OnBnClickedButtonSend6() { OnBnClickedButtonSend (5); }
void CDlgShortkey::OnBnClickedButtonSend7() { OnBnClickedButtonSend (6); }
void CDlgShortkey::OnBnClickedButtonSend8() { OnBnClickedButtonSend (7); }

void CDlgShortkey::OnBnClickedCheckAsciiHex1() { OnBnClickedCheckAsciiHex(0); }
void CDlgShortkey::OnBnClickedCheckAsciiHex2() { OnBnClickedCheckAsciiHex(1); }
void CDlgShortkey::OnBnClickedCheckAsciiHex3() { OnBnClickedCheckAsciiHex(2); }
void CDlgShortkey::OnBnClickedCheckAsciiHex4() { OnBnClickedCheckAsciiHex(3); }
void CDlgShortkey::OnBnClickedCheckAsciiHex5() { OnBnClickedCheckAsciiHex(4); }
void CDlgShortkey::OnBnClickedCheckAsciiHex6() { OnBnClickedCheckAsciiHex(5); }
void CDlgShortkey::OnBnClickedCheckAsciiHex7() { OnBnClickedCheckAsciiHex(6); }
void CDlgShortkey::OnBnClickedCheckAsciiHex8() { OnBnClickedCheckAsciiHex(7); }

void CDlgShortkey::OnCbnSelchangeComboShortkey1() { OnCbnSelchangeComboShortkey(0); }
void CDlgShortkey::OnCbnSelchangeComboShortkey2() { OnCbnSelchangeComboShortkey(1); }
void CDlgShortkey::OnCbnSelchangeComboShortkey3() { OnCbnSelchangeComboShortkey(2); }
void CDlgShortkey::OnCbnSelchangeComboShortkey4() { OnCbnSelchangeComboShortkey(3); }
void CDlgShortkey::OnCbnSelchangeComboShortkey5() { OnCbnSelchangeComboShortkey(4); }
void CDlgShortkey::OnCbnSelchangeComboShortkey6() { OnCbnSelchangeComboShortkey(5); }
void CDlgShortkey::OnCbnSelchangeComboShortkey7() { OnCbnSelchangeComboShortkey(6); }
void CDlgShortkey::OnCbnSelchangeComboShortkey8() { OnCbnSelchangeComboShortkey(7); }
void CDlgShortkey::OnCbnSelchangeComboShortkey9() { OnCbnSelchangeComboShortkey(8); }

void CDlgShortkey::OnBnClickedCheckCtrl1() { OnBnClickedCheckCtrl(0); }
void CDlgShortkey::OnBnClickedCheckCtrl2() { OnBnClickedCheckCtrl(1); }
void CDlgShortkey::OnBnClickedCheckCtrl3() { OnBnClickedCheckCtrl(2); }
void CDlgShortkey::OnBnClickedCheckCtrl4() { OnBnClickedCheckCtrl(3); }
void CDlgShortkey::OnBnClickedCheckCtrl5() { OnBnClickedCheckCtrl(4); }
void CDlgShortkey::OnBnClickedCheckCtrl6() { OnBnClickedCheckCtrl(5); }
void CDlgShortkey::OnBnClickedCheckCtrl7() { OnBnClickedCheckCtrl(6); }
void CDlgShortkey::OnBnClickedCheckCtrl8() { OnBnClickedCheckCtrl(7); }
void CDlgShortkey::OnBnClickedCheckCtrl9() { OnBnClickedCheckCtrl(8); }

void CDlgShortkey::OnBnClickedCheckAlt1()  { OnBnClickedCheckAlt(0); }
void CDlgShortkey::OnBnClickedCheckAlt2()  { OnBnClickedCheckAlt(1); }
void CDlgShortkey::OnBnClickedCheckAlt3()  { OnBnClickedCheckAlt(2); }
void CDlgShortkey::OnBnClickedCheckAlt4()  { OnBnClickedCheckAlt(3); }
void CDlgShortkey::OnBnClickedCheckAlt5()  { OnBnClickedCheckAlt(4); }
void CDlgShortkey::OnBnClickedCheckAlt6()  { OnBnClickedCheckAlt(5); }
void CDlgShortkey::OnBnClickedCheckAlt7()  { OnBnClickedCheckAlt(6); }
void CDlgShortkey::OnBnClickedCheckAlt8()  { OnBnClickedCheckAlt(7); }
void CDlgShortkey::OnBnClickedCheckAlt9()  { OnBnClickedCheckAlt(8); }

void CDlgShortkey::OnBnClickedCheckShift1() { OnBnClickedCheckShift(0); }
void CDlgShortkey::OnBnClickedCheckShift2() { OnBnClickedCheckShift(1); }
void CDlgShortkey::OnBnClickedCheckShift3() { OnBnClickedCheckShift(2); }
void CDlgShortkey::OnBnClickedCheckShift4() { OnBnClickedCheckShift(3); }
void CDlgShortkey::OnBnClickedCheckShift5() { OnBnClickedCheckShift(4); }
void CDlgShortkey::OnBnClickedCheckShift6() { OnBnClickedCheckShift(5); }
void CDlgShortkey::OnBnClickedCheckShift7() { OnBnClickedCheckShift(6); }
void CDlgShortkey::OnBnClickedCheckShift8() { OnBnClickedCheckShift(7); }
void CDlgShortkey::OnBnClickedCheckShift9() { OnBnClickedCheckShift(8); }


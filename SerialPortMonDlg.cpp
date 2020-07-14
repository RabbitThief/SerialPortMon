#include "stdafx.h"
#include <assert.h>
#include <Mmsystem.h>
#include "MultimediaTimer.h"
#include "SerialPortMon.h"
#include "SerialPortMonDlg.h"
#include "DlgConnection.h"
#include "DlgShortkey.h"
#include "DlgConfiguration.h"
#include "AppIni.h"
#include "escape_seq.h"
#include "ascii_str.h"
#include "Common.h"
#include "etc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCommPort *g_comm = NULL;

CSerialPortMonDlg::CSerialPortMonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSerialPortMonDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	_dlgShortkey = NULL;

	_connectPrev = -1;
	_commThread = NULL;
}

CSerialPortMonDlg::~CSerialPortMonDlg()
{
	if (_commThread) {
		delete _commThread;
	}
	if (g_comm) {
		g_comm->Close ();
		delete g_comm;
	}
	if (_dlgShortkey) {
		delete _dlgShortkey;
	}
}

void CSerialPortMonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT, _editInput);
	DDX_Control(pDX, IDC_EDIT_MESSAGE_LOG, _editMessage);
	DDX_Control(pDX, IDC_STATIC_HOMEPAGE, _staticHomepage);
}

BEGIN_MESSAGE_MAP(CSerialPortMonDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SEND, &CSerialPortMonDlg::OnDeltaposSpinSend)
	ON_BN_CLICKED(IDOK, &CSerialPortMonDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSerialPortMonDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SHORTKEY, &CSerialPortMonDlg::OnBnClickedButtonShortkey)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURATION, &CSerialPortMonDlg::OnBnClickedButtonConfiguration)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_CLOSE, &CSerialPortMonDlg::OnBnClickedButtonConnectClose)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CSerialPortMonDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CSerialPortMonDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSerialPortMonDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_RADIO_MESSAGE_HEX, &CSerialPortMonDlg::OnBnClickedRadioMessageHex)
	ON_BN_CLICKED(IDC_RADIO_RECORD_ASCII, &CSerialPortMonDlg::OnBnClickedRadioRecordAscii)
	ON_BN_CLICKED(IDC_RADIO_INPUT_HEX, &CSerialPortMonDlg::OnBnClickedRadioInputHex)
	ON_BN_CLICKED(IDC_RADIO_INPUT_ASCII, &CSerialPortMonDlg::OnBnClickedRadioInputAscii)
	ON_BN_CLICKED(IDC_CHECK_DISPLAY_SEND, &CSerialPortMonDlg::OnBnClickedCheckDisplaySend)
	ON_BN_CLICKED(IDC_CHECK_DISPLAY_RECV, &CSerialPortMonDlg::OnBnClickedCheckDisplayRecv)
	ON_WM_SIZING()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CSerialPortMonDlg::DlgInScreen ()
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

BOOL CSerialPortMonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetWindowPos (NULL, g_ini.mainWndSx, g_ini.mainWndSy, g_ini.mainWndDx, g_ini.mainWndDy, SWP_NOZORDER);
	DlgInScreen ();

	// 쇼트키 대화상자 생성
	_dlgShortkey = new CDlgShortkey (this);
	_dlgShortkey->Create (IDD_DIALOG_SHORTKEY);
	_dlgShortkey->ShowWindow (g_ini.options.SHOW_SHORTKEY_DLG ? SW_SHOW : SW_HIDE);

	// 모뎀 정보를 표시할 대화상자 생성 (Serial Connection 에서만 표시된다)
	_dlgModem.Create (IDD_DIALOG_MODEM, this);
	_dlgModem.SetWindowPos (NULL, 150, 7, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	_dlgModem.ShowWindow (SW_HIDE);

	// UDP 정보를 표시할 대화상자 생성 (UDP Connection 에서만 표시된다)
	_dlgUdp.Create (IDD_DIALOG_UDP, this);
	_dlgUdp.SetWindowPos (NULL, 150, 7, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	_dlgUdp.ShowWindow (SW_HIDE);

	// 메시지 입력 박스와 표시 박스의 폰트 변경
	_fontFixedsys.CreatePointFont(120,"Fixedsys");   
	_editMessage.SetFont(&_fontFixedsys);
	_editInput.SetFont(&_fontFixedsys);

	// 버젼 정보와 홈페이지 하이퍼링크 표시
	SetDlgItemText (IDC_STATIC_VERSION, GetVersionInfo(NULL, "ProductVersion"));

	_staticHomepage.SetURL("http://blog.daum.net/pg365/276");
	_staticHomepage.SetToolTipText("Serial/TCP/UDP 송수신 프로그램 웹페이지 방문");
	_staticHomepage.SetLinkCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));

	// 대화상자 컨트롤 초기화
	_editMessage.SetWindowText ("");
	_editMessage.SetMaxLines (g_ini.bufferLines);

	SetDlgItemText (IDC_BUTTON_CONNECT_CLOSE, "Connect");

	CheckDlgButton (IDC_CHECK_DISPLAY_SEND,		(g_ini.options.MESSAGE_DISPLAY_SEND)? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_DISPLAY_RECV,		(g_ini.options.MESSAGE_DISPLAY_RECV)? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_MESSAGE_HEX,      (g_ini.options.MESSAGE_DISPLAY_HEX) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_RECORD_ASCII,    !(g_ini.options.MESSAGE_DISPLAY_HEX) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_INPUT_HEX,        (g_ini.options.INPUT_SHOW_HEX)		? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_RADIO_INPUT_ASCII,     !(g_ini.options.INPUT_SHOW_HEX)		? BST_CHECKED : BST_UNCHECKED);

	// 처음 Edit박스에 문자열을 삽입할 때는 Escape sequence string으로 한다.
	_editInput._hexa = false;

	SetDlgItemText (IDC_EDIT_INPUT,  g_ini.input);
	SetDlgItemText (IDC_EDIT_PREFIX, g_ini.prefix);
	SetDlgItemText (IDC_EDIT_SUFFIX, g_ini.suffix);

	EnableWindowItem (FALSE);

	// 여기서 Escape sequence string 혹은 Hexadecimal string으로 바꾼다.
	OnBnClickedRadioInputHex();
	OnBnClickedRadioInputAscii();

	// 시작시 연결 옵션이 켜져 있다면 연결 시도
	if (g_ini.options.CONNECT_AT_STARTUP) {
		if (!OpenConnection ()) {
			DisplayCannotConnectMsg ();
		}
	}

	WordWrap ();

	SetTimer (1000, 33, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSerialPortMonDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSerialPortMonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSerialPortMonDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		if (IsWindowVisible ()) {
			_editMessage.Update ();
		}

		// 현재 연결 상태: -1 - g_comm 객체가 파괴된 상태(초기화), 0 - 연결 대기 중, 1 - 연결됨
		int connect = g_comm ? (g_comm->IsConnected () ? 1 : 0) : -1;

		// 상태가 바뀔 때만 실행
		if (_connectPrev != connect) {
			SetTitle ();
			EnableWindowItem (connect > 0);
			SetDlgItemText (IDC_BUTTON_CONNECT_CLOSE, (connect >= 0) ? "Close" : "Connect");

			_dlgModem.ShowWindow (dynamic_cast<CSerialCOM *>(g_comm) ? SW_SHOW : SW_HIDE);
			_dlgUdp  .ShowWindow (dynamic_cast<CUdp *>      (g_comm) ? SW_SHOW : SW_HIDE);
			
			if (g_ini.options.PLAY_SOUND) {
				if (connect ==  1) PlaySound (MAKEINTRESOURCE(IDR_WAVE_LOGON ),  AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC); 
				if (connect == -1) PlaySound (MAKEINTRESOURCE(IDR_WAVE_LOGOFF),  AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC); 
			}

			// 연결이 끊어질 때
			if (_connectPrev == 1 && connect == 0) {
				// TCP 서버가 아닌 경우 그냥 연결을 종료한다.
				// TCP 서버인 경우 연결을 기다려야 하기때문에 연결을 대기한다.
				if (g_ini.options.CONNECTION_TYPE != CONNECTION_TCPS) {
					CloseConnection ();
				}
			}
			_connectPrev = connect;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CSerialPortMonDlg::EnableWindowItem (BOOL enable)
{
	GetDlgItem (IDC_BUTTON_SEND)->EnableWindow (enable);
}

void CSerialPortMonDlg::OnDestroy()
{
	CloseConnection ();

	_dlgModem.DestroyWindow ();
	_dlgUdp.DestroyWindow ();

	_dlgShortkey->DestroyWindow ();

	// 메인 윈도우의 위치와 크기 저장
	RECT rectWnd;
	GetWindowRect (&rectWnd);
	g_ini.mainWndSx = rectWnd.left;
	g_ini.mainWndSy = rectWnd.top;
	g_ini.mainWndDx = rectWnd.right - rectWnd.left;
	g_ini.mainWndDy = rectWnd.bottom - rectWnd.top;

	CDialog::OnDestroy();
}

bool CSerialPortMonDlg::OnBnClickedOkCancel()
{
	g_ini.options.MESSAGE_DISPLAY_SEND  = IsDlgButtonChecked (IDC_CHECK_DISPLAY_SEND) == BST_CHECKED;
	g_ini.options.MESSAGE_DISPLAY_RECV  = IsDlgButtonChecked (IDC_CHECK_DISPLAY_RECV) == BST_CHECKED;
	g_ini.options.MESSAGE_DISPLAY_HEX	= IsDlgButtonChecked (IDC_RADIO_MESSAGE_HEX)  == BST_CHECKED;
	g_ini.options.INPUT_SHOW_HEX		= IsDlgButtonChecked (IDC_RADIO_INPUT_HEX)    == BST_CHECKED;

	// 마지막 Edit박스의 문자열을 저장할 때는 모두 Escape sequence string으로 변환하여
	// ini 파일에 저장한다.
	if (_editInput._hexa) {
		ChangeDlgItemTextFormat (IDC_EDIT_INPUT, false);
		ChangeDlgItemTextFormat (IDC_EDIT_PREFIX, false);
		ChangeDlgItemTextFormat (IDC_EDIT_SUFFIX, false);
	}

	GetDlgItemText (IDC_EDIT_INPUT,  g_ini.input, 256);
	GetDlgItemText (IDC_EDIT_PREFIX, g_ini.prefix, 16);
	GetDlgItemText (IDC_EDIT_SUFFIX, g_ini.suffix, 16);
	g_ini.input[256] = '\0';
	g_ini.prefix[16] = '\0';
	g_ini.suffix[16] = '\0';

	g_ini.options.SHOW_SHORTKEY_DLG = !!_dlgShortkey->IsWindowVisible ();
	return true;
}

void CSerialPortMonDlg::OnBnClickedOk()
{
	OnBnClickedOkCancel();
	OnOK();
}

void CSerialPortMonDlg::OnBnClickedCancel()
{
	OnBnClickedOkCancel();
	OnCancel();
}

void CSerialPortMonDlg::WordWrap ()
{
	CRect rect;
	_editMessage.GetWindowRect (&rect);
	ScreenToClient (&rect);

	DWORD style = _editMessage.GetStyle ();
	DWORD styleEx = _editMessage.GetExStyle ();

	CString text;
	_editMessage.GetWindowText (text);

	if (g_ini.options.DISPLAY_WORD_WRAP) {
		style &= ~(WS_HSCROLL | ES_AUTOHSCROLL);
	}
	else {
		style |= (WS_HSCROLL | ES_AUTOHSCROLL);
	}

	CWnd *wnd = _editMessage.GetNextWindow (GW_HWNDPREV);
	int id = _editMessage.GetDlgCtrlID ();

	_editMessage.DestroyWindow ();
	_editMessage.Create (style, rect, this, id);
	_editMessage.ModifyStyleEx   (0, styleEx, SWP_FRAMECHANGED);
	_editMessage.SetWindowText (text);
	_editMessage.SetWindowPos(wnd, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
}

void CSerialPortMonDlg::SetTitle ()
{
	int n = 0;
	char text[256 + 1];
	
	n += _snprintf (text + n, 256 - n, "SerialPortMon"); 

	if (g_comm) {
		/*
		// TCP Server일 때는 원격으로 접속한 client의 IP 주소를 보여준다.
		DWORD server_ip = 0;
		if (g_ini.options.CONNECTION_TYPE == CONNECTION_TCPS) {
			if (g_comm->IsConnected ()) {
				CTcpServer *com = dynamic_cast<CTcpServer *>(g_comm);
				server_ip = (com) ? com->GetRemoteIP () : 0;
			}
		}
		*/

		switch (g_ini.options.CONNECTION_TYPE) {
		case CONNECTION_UDP : n += _snprintf (text + n, 256 - n, " [UDP] Local IP: %s, Remote IP: %s, Port: %d", ip_addr_str(g_ini.localIpAddr),  ip_addr_str(g_ini.remoteIpAddr), (int)(WORD)g_ini.portNum); break;
		case CONNECTION_TCPS: n += _snprintf (text + n, 256 - n, " [TCP Server] Local IP: %s, Port: %d",         ip_addr_str(g_ini.localIpAddr),  (int)(WORD)g_ini.portNum); break;
		case CONNECTION_TCPC: n += _snprintf (text + n, 256 - n, " [TCP Client] Remote IP: %s, Port: %d",        ip_addr_str(g_ini.remoteIpAddr), (int)(WORD)g_ini.portNum); break;
		case CONNECTION_COM : n += _snprintf (text + n, 256 - n, " [Serial(COM)] %s, %d",                        g_ini.comPort, g_ini.comBps); break;
		}

		n += _snprintf (text + n, 256 - n, " - %s", 
			(g_comm->IsConnected ()) ? "Connected" : 
			((g_ini.options.CONNECTION_TYPE == CONNECTION_TCPS) ? "Waiting connection" : "Connecting"));
	}
	SetWindowText (text);
}

void CSerialPortMonDlg::DisplayCannotConnectMsg ()
{
	int n = 0;
	char text[256 + 1];

	if (g_ini.options.CONNECTION_TYPE == CONNECTION_TCPS) {
		n += _snprintf (text + n, 256 - n, "Cannot open ");
	}
	else {
		n += _snprintf (text + n, 256 - n, "Cannot connect to ");
	}

	switch (g_ini.options.CONNECTION_TYPE) {
	case CONNECTION_UDP : n += _snprintf (text + n, 256 - n, " [UDP] %s (%d)",         ip_addr_str(g_ini.remoteIpAddr),  (int)(WORD)g_ini.portNum); break;
	case CONNECTION_TCPS: n += _snprintf (text + n, 256 - n, " [TCP Server] %s (%d)",  ip_addr_str(GetLocalIP())      ,  (int)(WORD)g_ini.portNum); break;
	case CONNECTION_TCPC: n += _snprintf (text + n, 256 - n, " [TCP Client] %s (%d)",  ip_addr_str(g_ini.remoteIpAddr),  (int)(WORD)g_ini.portNum); break;
	case CONNECTION_COM : n += _snprintf (text + n, 256 - n, " [Serial(COM)] %s (%d)", g_ini.comPort, g_ini.comBps); break;
	}

	AfxMessageBox (text, MB_ICONEXCLAMATION|MB_OK);
}
 
int CSerialPortMonDlg::ReplaceCharCRLF (char *textchar, const char *msg, int msglen)
{
	if (g_ini.options.LINEFEED_WHEN_CR_LF) {
		int c = 0;
		for (int i=0; i<msglen; i++) {
			if ((msg[i] == '\r' && msg[i+1] != '\n') ||
				(msg[i] == '\n' && msg[i+1] != '\r')) {
				textchar[c++] = '\r'; 
				textchar[c++] = '\n'; 
			}
			else if ((msg[i] == '\r' && msg[i+1] == '\n') ||
				(msg[i] == '\n' && msg[i+1] == '\r')) {
				textchar[c++] = '\r'; 
				textchar[c++] = '\n'; 
				i++;
			}
			else {
				textchar[c++] = msg[i];
			}
		}
		return c;
	}
	else {
		memcpy (textchar, msg, msglen);
		return msglen;
	}
}

int CSerialPortMonDlg::ReplaceCharEscape (char *textchar, const char *msg, int msglen)
{
	return str2escseq (msg, msglen, textchar, msglen*5+9);
}

int CSerialPortMonDlg::ReplaceCharASCII (char *textchar, const char *msg, int msglen)
{
	int c = 0;
	for (int i=0; i<msglen; i++) {
		if ((0x00 <= msg[i] && msg[i] <= 0x1F) || msg[i] == 0x7F) {
			c += sprintf (textchar + c, "[%s]", pGetASCIIName(msg[i]));
		}
		else {
			textchar[c++] = msg[i];
		}
	}
	return c;
}

int CSerialPortMonDlg::ReplaceCharAs (char *textchar, char replace, const char *msg, int msglen)
{
	int c = 0;
	for (int i=0; i<msglen; i++) {
		// ASCII에서 표시 불가능한 문자
		if ((0x00 <= msg[i] && msg[i] <= 0x1F) || msg[i] == 0x7F) {
			textchar[c++] = replace;
		}
		else {
			textchar[c++] = msg[i];
		}
	}
	return c;
}

CString CSerialPortMonDlg::GetDisplayTime (int option)
{
	DWORD t = CMultimediaTimer::GetTime ();

    SYSTEMTIME st;
    GetSystemTime(&st);

	CString ret;
    
    printf("The system time is: %02d:%02d\n", st.wHour, st.wMinute);

	switch (option) {
	case 1: ret.Format ("[%04d/%02d/%02d]", (int)st.wYear, (int)st.wMonth, (int)st.wDay); break;
	case 2: ret.Format ("[%04d/%02d/%02d %02d:%02d:%02d]", (int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour, (int)st.wMinute, (int)st.wSecond); break;
	case 3: ret.Format ("[%02d:%02d:%02d]", (int)st.wHour, (int)st.wMinute, (int)st.wSecond); break;
	case 4: ret.Format ("[%02d:%02d:%02d.%03d]", (int)st.wHour, (int)st.wMinute, (int)st.wSecond, (int)st.wMilliseconds); break;
	case 5: ret.Format ("[%u.%03u]", (unsigned int)(t/1000), (unsigned int)(t%1000)); break;
	default: break;
	}
	return ret;
}

void CSerialPortMonDlg::LogMessage (const char *msg, int msgLen, bool send)
{
	int n = 0;
	char *buff = new char [msgLen*5+10];

	if (g_ini.options.MESSAGE_DISPLAY_HEX) {
		n = str2hexachar (msg, msgLen, buff, msgLen*5+9);
	}
	else {
		switch (g_ini.options.REPLACE_UNDISPLAY_CHAR) {
		case REPLACE_NOTUSE		: n = ReplaceCharCRLF	(buff,		msg, msgLen); break;
		case REPLACE_ESCAPE		: n = ReplaceCharEscape	(buff,		msg, msgLen); break;
		case REPLACE_ASCII		: n = ReplaceCharASCII	(buff,		msg, msgLen); break;
		case REPLACE_SPACE		: n = ReplaceCharAs		(buff, ' ', msg, msgLen); break;
		case REPLACE_UNDERSCORE	: n = ReplaceCharAs		(buff, '_', msg, msgLen); break;
		case REPLACE_PERIOD		: n = ReplaceCharAs		(buff, '.', msg, msgLen); break;
		default: assert(0); break;
		}
	}

	buff[n] = '\0';

	if (g_ini.options.DISPLAY_SENDRECV_MARK) {
		_editMessage.PrintLog (send ? ">" : "<");
	}

	if (g_ini.options.DISPLAY_SENDRECV_TIME > 0) {
		_editMessage.PrintLog (GetDisplayTime (g_ini.options.DISPLAY_SENDRECV_TIME));
	}

	_editMessage.PrintLog (buff);

	// 송수신 문자열 끝에서 자동 줄바꿈 체크되어 있을 때
	if (g_ini.options.LINEEND_AUTO_CRLF) {
		if ((n < 2) || (n >= 2 && !(buff[n-2] == '\r' && buff[n-1] == '\n'))) {
			_editMessage.PrintLog ("\r\n");
		}
	}
	delete [] buff;
}

void CSerialPortMonDlg::CloseConnection ()
{
	if (_commThread) {
		delete _commThread;
		_commThread = NULL;
	}
	if (g_comm) {
		g_comm->Close ();
		delete g_comm;
		g_comm = NULL;
	}
}

bool CSerialPortMonDlg::OpenConnection ()
{
	int timeout = 250; // ms

	CloseConnection ();

	switch (g_ini.options.CONNECTION_TYPE) {
	case CONNECTION_UDP : g_comm = new CUdp       (g_ini.localIpAddr, g_ini.remoteIpAddr,  (WORD)g_ini.portNum, timeout); break;
	case CONNECTION_TCPS: g_comm = new CTcpServer (g_ini.localIpAddr,                      (WORD)g_ini.portNum, timeout); break;
	case CONNECTION_TCPC: g_comm = new CTcpClient (                   g_ini.remoteIpAddr,  (WORD)g_ini.portNum, timeout); break;
	case CONNECTION_COM : g_comm = new CSerialCOM (g_ini.comPort,     g_ini.comBps,        (char)g_ini.comDatabits, 
		(char)g_ini.comParity, (char)g_ini.comStopbits, (eFlowControl)g_ini.comFlowcontrol, timeout); break;
	default: assert (0); break;
	}

	if (g_comm->Open ()) {
		g_comm->Connect (); 

		// 연결할 포트를 오픈 하였다면 계속 연결을 시도하거나 
		// 연결을 대기하거나 메시지 수신을 쓰레드로 수행한다.
		_commThread = new CCommThread (g_comm);
		return true;
	}
	else {
		delete g_comm;
		g_comm = NULL;
		return false;
	}
}

void CSerialPortMonDlg::OnBnClickedButtonConnectClose()
{
	if (g_comm) {
		CloseConnection ();
	}
	else {
		CDlgConnection dlg;

		int ret = dlg.DoModal ();
		if (ret == IDOK) {
			if (!OpenConnection ()) {
				DisplayCannotConnectMsg ();
			}
		}
	}
}

void CSerialPortMonDlg::OnBnClickedButtonShortkey()
{
	_dlgShortkey->ShowWindow (_dlgShortkey->IsWindowVisible () ? SW_HIDE : SW_SHOW);
	g_ini.options.SHOW_SHORTKEY_DLG = _dlgShortkey->IsWindowVisible () ? 1 : 0;
}

void CSerialPortMonDlg::OnBnClickedButtonConfiguration()
{
	CDlgConfiguration dlg;
	
	if (dlg.DoModal () == IDOK) {
		_editMessage.SetMaxLines (g_ini.bufferLines);
		WordWrap ();
	}
}

void CSerialPortMonDlg::OnBnClickedButtonSave()
{
	char szFilter[] = "Text file (*.txt)|*.m|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, "txt", "log.txt", OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, szFilter, this);
	
	if(IDOK == dlg.DoModal()) {
		CString contents;
		_editMessage.GetWindowText (contents);

		CString strPathName = dlg.GetPathName();
		FILE *fp = fopen ((LPCSTR)strPathName, "wb");
		if (fp) {
			fwrite ((LPCSTR)contents, 1, contents.GetLength (), fp);

			fclose (fp);
		}
	}
}

void CSerialPortMonDlg::OnBnClickedButtonClear()
{
	_editMessage.SetWindowText ("");
}

int CSerialPortMonDlg::GetDlgItemStr (int nIDEdit, char *str, int maxStr)
{
	CString buff;
	GetDlgItemText (nIDEdit, buff);

	if (_editInput._hexa) {
		return hexachar2str ((LPCSTR)buff, str, maxStr);
	}
	else {
		return escseq2str ((LPCSTR)buff, str, maxStr);
	}
}

void CSerialPortMonDlg::SendMessageSerial (char *msg, int len)
{
	if (g_ini.options.MESSAGE_DISPLAY_SEND) {
		LogMessage ((LPCSTR)msg, len, true);
	}

	if (g_comm && g_comm->IsConnected ()) {
		g_comm->Send (msg, len);
	}
}

void CSerialPortMonDlg::OnBnClickedButtonSend()
{
	char send[1024+1];
	int nSend = GetDlgItemStr (IDC_EDIT_INPUT, send, 1024);

	if (nSend > 0) {
		char prefix[16+1];
		char suffix[16+1];
		int nPrefix = GetDlgItemStr (IDC_EDIT_PREFIX, prefix, 16);
		int nSuffix = GetDlgItemStr (IDC_EDIT_SUFFIX, suffix, 16);

		int n = 0;
		char *all = new char [nPrefix + nSend + nSuffix + 1];

		memcpy (all + n, prefix, nPrefix);	n += nPrefix;
		memcpy (all + n, send,   nSend);	n += nSend;
		memcpy (all + n, suffix, nSuffix);	n += nSuffix;
		all[n] = '\0';

		SendMessageSerial (all, n);

		delete [] all;

		// HEXA로 표시되어 있다면 ASCII로 바꾸어 읽는다.
		if (_editInput._hexa) {
			ChangeDlgItemTextFormat (IDC_EDIT_INPUT, false);
		}

		// 읽어서 스크롤 텍스트에 저장한다.
		CString input;
		GetDlgItemText (IDC_EDIT_INPUT, input);

		_editInput.AddScrollText((LPCSTR)input);
		_editInput.SetWindowText ("");

	}
	_editInput.SetFocus ();
}

void CSerialPortMonDlg::OnBnClickedRadioMessageHex()
{
	g_ini.options.MESSAGE_DISPLAY_HEX = IsDlgButtonChecked (IDC_RADIO_MESSAGE_HEX)  == BST_CHECKED;
}

void CSerialPortMonDlg::OnBnClickedRadioRecordAscii()
{
	g_ini.options.MESSAGE_DISPLAY_HEX = IsDlgButtonChecked (IDC_RADIO_RECORD_ASCII)  != BST_CHECKED;
}

void CSerialPortMonDlg::ChangeDlgItemTextFormat (int nIDEdit, bool hexa)
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
		char *escseq = new char [2*buff.GetLength () + 10];
		int n = hexachar2escseq ((LPCSTR)buff, escseq, 2*buff.GetLength () + 9);
		escseq[n] = '\0';

		SetDlgItemText (nIDEdit, escseq);
		delete [] escseq;
	}
}

void CSerialPortMonDlg::OnBnClickedRadioInput()
{
	g_ini.options.INPUT_SHOW_HEX = IsDlgButtonChecked (IDC_RADIO_INPUT_HEX) == BST_CHECKED;

	bool inputHexa = g_ini.options.INPUT_SHOW_HEX ? true : false;

	if (_editInput._hexa != inputHexa) {
		ChangeDlgItemTextFormat (IDC_EDIT_INPUT,  inputHexa);
		ChangeDlgItemTextFormat (IDC_EDIT_PREFIX, inputHexa);
		ChangeDlgItemTextFormat (IDC_EDIT_SUFFIX, inputHexa);

		_editInput._hexa = inputHexa;
	}

	_editInput.SetFocus ();
}

void CSerialPortMonDlg::OnBnClickedRadioInputHex()   { OnBnClickedRadioInput(); }
void CSerialPortMonDlg::OnBnClickedRadioInputAscii() { OnBnClickedRadioInput(); }

void CSerialPortMonDlg::OnDeltaposSpinSend(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	// TODO: Add your control notification handler code here
	if (pNMUpDown->iDelta > 0) _editInput.KeyDown ();
	else                       _editInput.KeyUp ();

	*pResult = 0;
}

void CSerialPortMonDlg::OnBnClickedCheckDisplaySend()
{
	g_ini.options.MESSAGE_DISPLAY_SEND = IsDlgButtonChecked (IDC_CHECK_DISPLAY_SEND) == BST_CHECKED;
}

void CSerialPortMonDlg::OnBnClickedCheckDisplayRecv()
{
	g_ini.options.MESSAGE_DISPLAY_RECV = IsDlgButtonChecked (IDC_CHECK_DISPLAY_RECV) == BST_CHECKED;
}

void CSerialPortMonDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	// 여기다 크기 제한하는 코드 추가함
	if (pRect->right - pRect->left < 700) pRect->right = pRect->left + 700;
	if (pRect->bottom - pRect->top < 500) pRect->bottom = pRect->top + 500;

	CDialog::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
}

void CSerialPortMonDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (GetDlgItem(IDC_EDIT_MESSAGE_LOG) == NULL){
		// 대화상자의 아이템들이 생성되기 전이다.
		return;
	}

	//if (_dlgModem.m_hWnd) _dlgModem.SetWindowPos (NULL, cx-700+150, 7, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	//if (_dlgUdp  .m_hWnd) _dlgUdp  .SetWindowPos (NULL, cx-700+150, 7, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// 좌우 정렬
	GetDlgItem(IDC_EDIT_MESSAGE_LOG)->SetWindowPos (NULL, 11, 42, cx-22, cy-220, SWP_NOZORDER | SWP_NOREDRAW);
	
	// 왼쪽 정렬
	GetDlgItem(IDC_STATIC1			 )->SetWindowPos (NULL,  22, cy-166, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_RADIO_MESSAGE_HEX )->SetWindowPos (NULL, 123, cy-168, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_RADIO_RECORD_ASCII)->SetWindowPos (NULL, 168, cy-168, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_CHECK_DISPLAY_SEND)->SetWindowPos (NULL, 244, cy-168, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_CHECK_DISPLAY_RECV)->SetWindowPos (NULL, 363, cy-168, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);

	// 오른쪽 정렬
	GetDlgItem(IDC_BUTTON_SAVE		)->SetWindowPos (NULL, cx-180, cy-171, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_BUTTON_CLEAR		)->SetWindowPos (NULL, cx -89, cy-171, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);

	// 좌우 정렬
	GetDlgItem(IDC_STATIC2			)->SetWindowPos (NULL, 11    , cy-140, cx- 22, 85, SWP_NOZORDER | SWP_NOREDRAW);		
	GetDlgItem(IDC_EDIT_INPUT		)->SetWindowPos (NULL, 22    , cy-120, cx-130, 23, SWP_NOZORDER | SWP_NOREDRAW);		
	GetDlgItem(IDC_SPIN_SEND		)->SetWindowPos (NULL, cx-108, cy-120, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_BUTTON_SEND		)->SetWindowPos (NULL, cx- 88, cy-120, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);

	// 왼쪽 정렬
	GetDlgItem(IDC_STATIC3			)->SetWindowPos (NULL,  22, cy-85, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_RADIO_INPUT_HEX	)->SetWindowPos (NULL, 124, cy-86, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_RADIO_INPUT_ASCII)->SetWindowPos (NULL, 167, cy-86, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	
	// 오른쪽 정렬
	GetDlgItem(IDC_STATIC4			)->SetWindowPos (NULL, cx-340, cy-85, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_EDIT_PREFIX		)->SetWindowPos (NULL, cx-230, cy-89, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_STATIC5			)->SetWindowPos (NULL, cx-160, cy-85, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_EDIT_SUFFIX		)->SetWindowPos (NULL, cx- 89, cy-89, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);

	// 왼쪽 정렬
	GetDlgItem(IDC_STATIC_VERSION	)->SetWindowPos (NULL, 11, cy-45, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_STATIC_HOMEPAGE	)->SetWindowPos (NULL, 11, cy-25, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);

	// 오른쪽 정렬
	GetDlgItem(IDC_BUTTON_SHORTKEY		)->SetWindowPos (NULL, 330, cy-33, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDC_BUTTON_CONFIGURATION	)->SetWindowPos (NULL, 430, cy-33, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	GetDlgItem(IDCANCEL					)->ShowWindow   (SW_HIDE);
	GetDlgItem(IDOK						)->SetWindowPos (NULL, cx-89, cy-33, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);

	InvalidateRect (NULL, TRUE);
}

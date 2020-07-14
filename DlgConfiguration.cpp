#include "stdafx.h"
#include "SerialPortMon.h"
#include "DlgConfiguration.h"
#include "AppIni.h"
#include "Common.h"


IMPLEMENT_DYNAMIC(CDlgConfiguration, CDialog)

CDlgConfiguration::CDlgConfiguration(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConfiguration::IDD, pParent)
{
}

CDlgConfiguration::~CDlgConfiguration()
{
}

void CDlgConfiguration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_BUFFER_LINES, _spinBufferLines);
}


BEGIN_MESSAGE_MAP(CDlgConfiguration, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgConfiguration::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgConfiguration::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_REPLACE_CHAR, &CDlgConfiguration::OnCbnSelchangeComboReplaceChar)
END_MESSAGE_MAP()


BOOL CDlgConfiguration::OnInitDialog()
{
	CDialog::OnInitDialog();

	_spinBufferLines.SetRange (80, 10000);

	CheckDlgButton (IDC_CHECK_CONNECT_AT_STARTUP,   g_ini.options.CONNECT_AT_STARTUP	? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_PLAY_SOUND,			g_ini.options.PLAY_SOUND			? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_7BITS_ASCII,			g_ini.options.ASCII_7BITS			? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_LINEND_CRLF,			g_ini.options.LINEEND_AUTO_CRLF		? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_LINEFEED_CR_LF,		g_ini.options.LINEFEED_WHEN_CR_LF	? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_SENDRECV_MARK,		g_ini.options.DISPLAY_SENDRECV_MARK	? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_AUTO_WORD_WRAP,		g_ini.options.DISPLAY_WORD_WRAP		? BST_CHECKED : BST_UNCHECKED);

	SetDlgItemInt      (IDC_EDIT_BUFFER_LINES,						BOUND (g_ini.bufferLines, 80, 80000));			
	SendDlgItemMessage (IDC_COMBO_REPLACE_CHAR,		CB_SETCURSEL,	BOUND (g_ini.options.REPLACE_UNDISPLAY_CHAR, 0, 5));
	SendDlgItemMessage (IDC_COMBO_TIME_DISPLAY,		CB_SETCURSEL,	BOUND (g_ini.options.DISPLAY_SENDRECV_TIME, 0, 5));

	OnCbnSelchangeComboReplaceChar();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfiguration::OnBnClickedOk()
{
	g_ini.options.CONNECT_AT_STARTUP	= IsDlgButtonChecked (IDC_CHECK_CONNECT_AT_STARTUP)	== BST_CHECKED;
	g_ini.options.PLAY_SOUND			= IsDlgButtonChecked (IDC_CHECK_PLAY_SOUND)			== BST_CHECKED;
	g_ini.options.ASCII_7BITS			= IsDlgButtonChecked (IDC_CHECK_7BITS_ASCII)		== BST_CHECKED;
	g_ini.options.LINEEND_AUTO_CRLF		= IsDlgButtonChecked (IDC_CHECK_LINEND_CRLF)		== BST_CHECKED;
	g_ini.options.LINEFEED_WHEN_CR_LF	= IsDlgButtonChecked (IDC_CHECK_LINEFEED_CR_LF)		== BST_CHECKED;
	g_ini.options.DISPLAY_SENDRECV_MARK	= IsDlgButtonChecked (IDC_CHECK_SENDRECV_MARK)		== BST_CHECKED;
	g_ini.options.DISPLAY_WORD_WRAP		= IsDlgButtonChecked (IDC_CHECK_AUTO_WORD_WRAP)		== BST_CHECKED;

	g_ini.bufferLines					= BOUND (GetDlgItemInt (IDC_EDIT_BUFFER_LINES), 80, 80000);
	g_ini.options.REPLACE_UNDISPLAY_CHAR= BOUND (SendDlgItemMessage (IDC_COMBO_REPLACE_CHAR, CB_GETCURSEL), 0, 5);
	g_ini.options.DISPLAY_SENDRECV_TIME	= BOUND (SendDlgItemMessage (IDC_COMBO_TIME_DISPLAY, CB_GETCURSEL), 0, 5);

	OnOK();
}

void CDlgConfiguration::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgConfiguration::OnCbnSelchangeComboReplaceChar()
{
	int replaceChar = SendDlgItemMessage (IDC_COMBO_REPLACE_CHAR, CB_GETCURSEL);
	if (replaceChar == 0) {
		GetDlgItem (IDC_CHECK_LINEFEED_CR_LF)->EnableWindow (TRUE);
	}
	else {
		CheckDlgButton (IDC_CHECK_LINEFEED_CR_LF, BST_UNCHECKED);
		GetDlgItem (IDC_CHECK_LINEFEED_CR_LF)->EnableWindow (FALSE);
	}
}

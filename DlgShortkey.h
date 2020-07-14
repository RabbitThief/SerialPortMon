#pragma once
#include "virtual_key.h"

class CDlgShortkey : public CDialog
{
	DECLARE_DYNAMIC(CDlgShortkey)

public:
	CDlgShortkey(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgShortkey();

	void OnKeyEvent (sVirtualKey vk);

	enum { IDD = IDD_DIALOG_SHORTKEY };

private:
	int  _enable_prev;
	bool _message_hexa[8];
	int   _dlg_cx;
	int   _dlg_cy;

	HBITMAP HB_LAMP_DIS;
	HBITMAP HB_LAMP_OFF;
	HBITMAP HB_LAMP_ON;

	HBITMAP SelectBitmap (bool connected, bool flag);
	BOOL IsObjDeleteable(HGDIOBJ hGdiObj);
	BOOL ReplaceStaticControlBitmap (int ControlID, HBITMAP hBitmap);

	void DlgInScreen ();
	void ChangeDlgItemTextFormat (int nIDEdit, bool hexa);
	void EnableWindowItem (BOOL enable);

	void OnBnClickedCheckAsciiHex(int i);
	void OnBnClickedButtonSend(int i);
	void OnCbnSelchangeComboShortkey(int i);
	void OnBnClickedCheckCtrl(int i);
	void OnBnClickedCheckAlt(int i);
	void OnBnClickedCheckShift(int i);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedCheckHideShortkey();
	afx_msg void OnBnClickedButtonSend1();
	afx_msg void OnBnClickedButtonSend2();
	afx_msg void OnBnClickedButtonSend3();
	afx_msg void OnBnClickedButtonSend4();
	afx_msg void OnBnClickedButtonSend5();
	afx_msg void OnBnClickedButtonSend6();
	afx_msg void OnBnClickedButtonSend7();
	afx_msg void OnBnClickedButtonSend8();
	afx_msg void OnBnClickedCheckAsciiHex1();
	afx_msg void OnBnClickedCheckAsciiHex2();
	afx_msg void OnBnClickedCheckAsciiHex3();
	afx_msg void OnBnClickedCheckAsciiHex4();
	afx_msg void OnBnClickedCheckAsciiHex5();
	afx_msg void OnBnClickedCheckAsciiHex6();
	afx_msg void OnBnClickedCheckAsciiHex7();
	afx_msg void OnBnClickedCheckAsciiHex8();
	afx_msg void OnCbnSelchangeComboShortkey1();
	afx_msg void OnCbnSelchangeComboShortkey2();
	afx_msg void OnCbnSelchangeComboShortkey3();
	afx_msg void OnCbnSelchangeComboShortkey4();
	afx_msg void OnCbnSelchangeComboShortkey5();
	afx_msg void OnCbnSelchangeComboShortkey6();
	afx_msg void OnCbnSelchangeComboShortkey7();
	afx_msg void OnCbnSelchangeComboShortkey8();
	afx_msg void OnCbnSelchangeComboShortkey9();
	afx_msg void OnBnClickedCheckCtrl1();
	afx_msg void OnBnClickedCheckCtrl2();
	afx_msg void OnBnClickedCheckCtrl3();
	afx_msg void OnBnClickedCheckCtrl4();
	afx_msg void OnBnClickedCheckCtrl5();
	afx_msg void OnBnClickedCheckCtrl6();
	afx_msg void OnBnClickedCheckCtrl7();
	afx_msg void OnBnClickedCheckCtrl8();
	afx_msg void OnBnClickedCheckCtrl9();
	afx_msg void OnBnClickedCheckAlt1();
	afx_msg void OnBnClickedCheckAlt2();
	afx_msg void OnBnClickedCheckAlt3();
	afx_msg void OnBnClickedCheckAlt4();
	afx_msg void OnBnClickedCheckAlt5();
	afx_msg void OnBnClickedCheckAlt6();
	afx_msg void OnBnClickedCheckAlt7();
	afx_msg void OnBnClickedCheckAlt8();
	afx_msg void OnBnClickedCheckAlt9();
	afx_msg void OnBnClickedCheckShift1();
	afx_msg void OnBnClickedCheckShift2();
	afx_msg void OnBnClickedCheckShift3();
	afx_msg void OnBnClickedCheckShift4();
	afx_msg void OnBnClickedCheckShift5();
	afx_msg void OnBnClickedCheckShift6();
	afx_msg void OnBnClickedCheckShift7();
	afx_msg void OnBnClickedCheckShift8();
	afx_msg void OnBnClickedCheckShift9();
};

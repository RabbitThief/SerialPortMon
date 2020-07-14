#pragma once
#include "Ini.h"
#include "virtual_key.h"

enum eConnectionType {
	CONNECTION_UDP,		// UDP, 
	CONNECTION_TCPS,	// TCP Server, 
	CONNECTION_TCPC,	// TCP Client, 
	CONNECTION_COM,		// Serial(COM).
};

enum eReplaceChar {
	REPLACE_NOTUSE,		// 사용안함
	REPLACE_ESCAPE,		// Escape Sequence
	REPLACE_ASCII,		// ASCII 약자
	REPLACE_SPACE,		// 공백 문자( )
	REPLACE_UNDERSCORE,	// 믿줄 문자(_)
	REPLACE_PERIOD,		// 마침표 문자(.)
};

struct sBitOptions {
	DWORD MESSAGE_DISPLAY_SEND	: 1;	// 주 대화상자에서 Send Message를 로그 창에 기록 할 지 여부를 결정
	DWORD MESSAGE_DISPLAY_RECV	: 1;	// 주 대화상자에서 Recv Message를 로그 창에 기록 할 지 여부를 결정
	DWORD MESSAGE_DISPLAY_HEX	: 1;	// 주 대화상자에 표시되는 메시지 형식을 HEXA 코드로 할지 ASCII 문자로 할지 결정
	DWORD INPUT_SHOW_HEX		: 1;	// Send 입력 박스에 표시되는 메시지 형식을 HEXA 코드로 할지 ASCII 문자로 할지 결정, 여기서 ASCII 문자열에는 Escape sequence char.를 사용가능
	DWORD SHOW_SHORTKEY_DLG		: 1;	// 단축키 입력 대화상자를 표시할 지 여부를 결정
	DWORD HIDE_SHORTKEY_PART	: 1;	// 단축키 입력 대화상자에서 단축키 부분을 보여줄 지 결정
	DWORD CONNECTION_TYPE		: 2;	// 연결 종류 결정: eConnectionType 참조
	DWORD PLAY_SOUND			: 1;	// 설정 대화상자: 연결 되거나 연결이 종료될 때 사운드로 알림 기능
	DWORD ASCII_7BITS			: 1;	// 설정 대화상자: Recv Message에 7-bit 마스크를 씌울 지 결정
	DWORD CONNECT_AT_STARTUP	: 1;	// 설정 대화상자: 프로그램 실행시 연결 설정으로 연결
	DWORD LINEEND_AUTO_CRLF		: 1;	// 설정 대화상자: 송수신 문자열 끝에서 자동 줄바꿈
	DWORD LINEFEED_WHEN_CR_LF	: 1;	// 설정 대화상자: 줄바꿈 문자를 만나면 줄바꿈
	DWORD REPLACE_UNDISPLAY_CHAR: 4;	// 설정 대화상자: 표시할 수 없는 문자 대체
	DWORD DISPLAY_SENDRECV_MARK	: 1;
	DWORD DISPLAY_SENDRECV_TIME	: 3; 
	DWORD DISPLAY_WORD_WRAP		: 1;
};

struct sMsgOpt {
	DWORD SHORTKEY_DISPLAY_HEX : 1;		// 단축키 입력 대화상자에 표시되는 문자열을 HEXA 코드로 할지 ASCII 문자로 할지 결정
};

class CAppIni: public CIni
{
public: 
	int		mainWndSx;
	int		mainWndSy;
	int		mainWndDx;
	int		mainWndDy;

	int		shortkeyWndSx;
	int		shortkeyWndSy;

	sBitOptions options;

	char	prefix[16+1];
	char	suffix[16+1];
	char	input[256+1];

	char	message[8][256+1];
	sMsgOpt msgOpt[8];
	DWORD	delay[8];
	DWORD	repeat[8];
	sVirtualKey shortKey[9];
	DWORD	bufferLines;

	char	comPort[MAX_PATH+1];
	int		comBps;
	char	comDatabits;
	char	comParity;
	char	comStopbits;
	char	comFlowcontrol;
	DWORD	remoteIpAddr;
	DWORD	localIpAddr;
	DWORD	portNum;

public:
	CAppIni(const char *ini_path);
	virtual ~CAppIni();

	void LoadData();
	void SaveData();

private:
	void TransferAllData (bool bSave);
};

extern CAppIni g_ini;

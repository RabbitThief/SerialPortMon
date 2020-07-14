#include "stdafx.h"
#include <ctype.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

struct ASCIITable {
	char code;
	const char *name;
	const char *description;
	const char *hanDesc;
} _asciiTable[] = {
	{ 0x00, "NUL", "Null Character",			"공백 문자"							}, 
	{ 0x01, "SOH", "Start of Header",			"헤더 시작"							}, 
	{ 0x02, "STX", "Start of Text",				"본문 시작, 헤더 종료"				}, 
	{ 0x03, "ETX", "End of Text",				"본문 종료"							}, 
	{ 0x04, "EOT", "End of Transmission",		"전송 종료, 데이터 링크 초기화"		}, 
	{ 0x05, "ENQ", "Enquiry",					"응답 요구"							}, 
	{ 0x06, "ACK", "Acknowledgment",			"긍정응답"							}, 
	{ 0x07, "BEL", "Bell",						"경고음"							}, 
	{ 0x08, "BS",  "Backspace",					"백스페이스"						}, 
	{ 0x09, "HT",  "Horizontal Tab",			"수평 탭"							}, 
	{ 0x0A, "LF",  "Line feed",					"개행"								}, 
	{ 0x0B, "VT",  "Vertical Tab",				"수직 탭"							}, 
	{ 0x0C, "FF",  "Form feed",					"다음 페이지"						}, 
	{ 0x0D, "CR",  "Carriage return",			"복귀"								}, 
	{ 0x0E, "SO",  "Shift Out",					"확장문자 시작"						}, 
	{ 0x0F, "SI",  "Shift In",					"확장문자 종료"						}, 
	{ 0x10, "DLE", "Data Link Escape",			"전송 제어 확장"					}, 
	{ 0x11, "DC1", "Device Control 1",			"장치 제어 1"						}, 
	{ 0x12, "DC2", "Device Control 2",			"장치 제어 2"						}, 
	{ 0x13, "DC3", "Device Control 3",			"장치 제어 3"						}, 
	{ 0x14, "DC4", "Device Control 4",			"장치 제어 4"						}, 
	{ 0x15, "NAK", "Negative Acknowledgement",	"부정응답"							}, 
	{ 0x16, "SYN", "Synchronous idle",			"동기"								}, 
	{ 0x17, "ETB", "End of Transmission Block", "전송블록 종료"						}, 
	{ 0x18, "CAN", "Cancel",					"무시"								}, 
	{ 0x19, "EM",  "End of Medium",				"매체 종료"							}, 
	{ 0x1A, "SUB", "Substitute",				"치환"								}, 
	{ 0x1B, "ESC", "Escape",					"제어기능 추가"						}, 
	{ 0x1C, "FS",  "File Separator",			"파일경계 할당"						}, 
	{ 0x1D, "GS",  "Group Separator",			"레코드 그룹경계 할당"				}, 
	{ 0x1E, "RS",  "Record Separator",			"레코드 경계 할당"					}, 
	{ 0x1F, "US",  "Unit Separator",			"장치 경계 할당"					}, 
	{ 0x7F, "DEL", "Delete",					"삭제"								},
};

const char *pGetASCIIName (int code)
{
	static char buff[2] = {0, 0};

	if (0x00 <= code && code <= 0x1F) return _asciiTable[code].name;
	else if (code == 0x7F) return _asciiTable[code].name;
	else { buff[0] = (char)code; return buff; }
}
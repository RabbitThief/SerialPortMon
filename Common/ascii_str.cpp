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
	{ 0x00, "NUL", "Null Character",			"���� ����"							}, 
	{ 0x01, "SOH", "Start of Header",			"��� ����"							}, 
	{ 0x02, "STX", "Start of Text",				"���� ����, ��� ����"				}, 
	{ 0x03, "ETX", "End of Text",				"���� ����"							}, 
	{ 0x04, "EOT", "End of Transmission",		"���� ����, ������ ��ũ �ʱ�ȭ"		}, 
	{ 0x05, "ENQ", "Enquiry",					"���� �䱸"							}, 
	{ 0x06, "ACK", "Acknowledgment",			"��������"							}, 
	{ 0x07, "BEL", "Bell",						"�����"							}, 
	{ 0x08, "BS",  "Backspace",					"�齺���̽�"						}, 
	{ 0x09, "HT",  "Horizontal Tab",			"���� ��"							}, 
	{ 0x0A, "LF",  "Line feed",					"����"								}, 
	{ 0x0B, "VT",  "Vertical Tab",				"���� ��"							}, 
	{ 0x0C, "FF",  "Form feed",					"���� ������"						}, 
	{ 0x0D, "CR",  "Carriage return",			"����"								}, 
	{ 0x0E, "SO",  "Shift Out",					"Ȯ�幮�� ����"						}, 
	{ 0x0F, "SI",  "Shift In",					"Ȯ�幮�� ����"						}, 
	{ 0x10, "DLE", "Data Link Escape",			"���� ���� Ȯ��"					}, 
	{ 0x11, "DC1", "Device Control 1",			"��ġ ���� 1"						}, 
	{ 0x12, "DC2", "Device Control 2",			"��ġ ���� 2"						}, 
	{ 0x13, "DC3", "Device Control 3",			"��ġ ���� 3"						}, 
	{ 0x14, "DC4", "Device Control 4",			"��ġ ���� 4"						}, 
	{ 0x15, "NAK", "Negative Acknowledgement",	"��������"							}, 
	{ 0x16, "SYN", "Synchronous idle",			"����"								}, 
	{ 0x17, "ETB", "End of Transmission Block", "���ۺ�� ����"						}, 
	{ 0x18, "CAN", "Cancel",					"����"								}, 
	{ 0x19, "EM",  "End of Medium",				"��ü ����"							}, 
	{ 0x1A, "SUB", "Substitute",				"ġȯ"								}, 
	{ 0x1B, "ESC", "Escape",					"������ �߰�"						}, 
	{ 0x1C, "FS",  "File Separator",			"���ϰ�� �Ҵ�"						}, 
	{ 0x1D, "GS",  "Group Separator",			"���ڵ� �׷��� �Ҵ�"				}, 
	{ 0x1E, "RS",  "Record Separator",			"���ڵ� ��� �Ҵ�"					}, 
	{ 0x1F, "US",  "Unit Separator",			"��ġ ��� �Ҵ�"					}, 
	{ 0x7F, "DEL", "Delete",					"����"								},
};

const char *pGetASCIIName (int code)
{
	static char buff[2] = {0, 0};

	if (0x00 <= code && code <= 0x1F) return _asciiTable[code].name;
	else if (code == 0x7F) return _asciiTable[code].name;
	else { buff[0] = (char)code; return buff; }
}
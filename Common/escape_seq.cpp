#include "stdafx.h"
#include <ctype.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

struct EscapeSequence {
	const char *escapeString;
	uchar escapeChar;
	const char *description;
} _escapeSequences[] = {
	{ "\\a",	'\a',	"0x07 - Bell (alert)"		},
	{ "\\b",	'\b',	"0x08 - Backspace"			},
	{ "\\f",	'\f',	"0x0C - Formfeed"			},
	{ "\\n",	'\n',	"0x0A - New line"			},
	{ "\\r",	'\r',	"0x0D - Carriage return"	},
	{ "\\t",	'\t',	"0x09 - Horizontal tab"	},
	{ "\\v",	'\v',	"0x0B - Vertical tab"		},
	{ "\\'",	'\'',	"0x27 - Single quotation mark"	},
	{ "\\\"",	'\"',	"0x22 - Double quotation mark"	},
	{ "\\\\",	'\\',	"0x5C - Backslash"				},
	{ "\\?",	'\?',	"0x3F - Literal question mark"	},
//	{ "\\ooo",	'\0',	"0x00 - ASCII character in octal notation"	},
//	{ "\\xhh",	'\x0',	"0x00 - ASCII character in hexadecimal notation" },
};

int _n_escapeSequences = sizeof(_escapeSequences)/sizeof(EscapeSequence);

inline bool isodigit(char ch) 
{ 
	return (ch >= '0' && ch <= '7'); 
}

inline int hexachar2num (char ch)
{
	if ('0' <= ch && ch <= '9') return ch - '0';
	if ('A' <= ch && ch <= 'F') return ch - 'A' + 10;
	if ('a' <= ch && ch <= 'f') return ch - 'a' + 10;
	return 0;
}

inline char num2hexachar (uint dec)
{
	if      (0  <= dec && dec <= 9 ) return dec + '0';
	else if (10 <= dec && dec <= 15) return dec + 'A' - 10;
	else                             return '0';
}

int str2hexachar (const char *str, int str_len, char *hexachar, int hexachar_len)
{
	int n = 0;

	for (int i=0; i<str_len; i++) {
		n += _snprintf (hexachar + n, hexachar_len - n, "%02X", (uint)(uchar)str[i]);
		if (i+1 < str_len) n += _snprintf (hexachar + n, hexachar_len - n, " ");
	}
	return n;
}

int hexachar2str (const char *hexachar, char *str, int str_len)
{
	// "00 11 22 3 4 567890"을 { 0x00, 0x11, 0x22, 0x03, 0x04, 0x56, 0x78, 0x90 }으로 바꾼다.
	int length = 0;
	const uchar *p = (uchar *)hexachar;

	while (*p && length < str_len) {
		// Skip space char.
		while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;

		if (p[0] == '\0') {
			break;
		}
		else if (isxdigit(p[0]) && isxdigit(p[1])) {
			str[length] = (uchar)(16*hexachar2num(p[0]) + hexachar2num(p[1]));
			++length; p+=2;
		}
		else if (isxdigit(p[0])) {
			str[length] = (uchar)(hexachar2num(p[0]));
			++length; p+=1;
		}
		else {
			p++;
		}
	}
	return length;
}

int str2escseq (const char *str, int str_len, char *escseq, int escseq_len)
{
	int n = 0;
	const uchar *p = (uchar *)str;

	for (int i=0; i<str_len; i++) {
		if (0x20 <= p[i] && p[i] <= 0x7E) { if (n < escseq_len) { escseq[n] = p[i]; n++; } }
		else if (p[i] == '\a')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = 'a' ; n+=2; } }
		else if (p[i] == '\b')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = 'b' ; n+=2; } }
		else if (p[i] == '\f')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = 'f' ; n+=2; } }
		else if (p[i] == '\n')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = 'n' ; n+=2; } }
		else if (p[i] == '\r')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = 'r' ; n+=2; } }
		else if (p[i] == '\t')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = 't' ; n+=2; } }
		else if (p[i] == '\v')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = 'v' ; n+=2; } }
		else if (p[i] == '\'')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = '\''; n+=2; } }
		else if (p[i] == '\"')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = '\"'; n+=2; } }
		else if (p[i] == '\\')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = '\\'; n+=2; } }
		else if (p[i] == '\?')  { if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = '?' ; n+=2; } }
		else if (p[i] == '\0' && !isodigit(p[i+1]))  { 
			if (n+1 < escseq_len) { escseq[n] = '\\'; escseq[n+1] = '0' ; n+=2; } 
		}
		else if (p[i] >= 0x80 && p[i+1] >= 0x80)  { // 한글은 변환하지 않는다.
			if (n+2 < escseq_len) { escseq[n] = p[i]; escseq[n+1] = p[i+1]; n+=2; i++; } 
		}
		else {
			if (n+3 < escseq_len) { 
				escseq[n+0] = '\\'; 
				escseq[n+1] = 'x'; 
				escseq[n+2] = num2hexachar ((p[i]>>4)&0x0F); 
				escseq[n+3] = num2hexachar ((p[i]>>0)&0x0F);
				n += 4;
			}
		}
	}
	return n;
}

int escseq2str (const char *escseq, char *str, int str_len)
{
	// Escape sequence가 포함된 문자열을 일반 문자열로 변환한다.
	int length = 0;
	const char *p = escseq;

	while (*p && length < str_len) {
		if      (p[0] == '\0') break;
		else if (p[0] == '\\') {	// Escape Sequence
			if      (p[1] == 'a')  { str[length] = 0x07;	++length; p+=2;	}
			else if (p[1] == 'b')  { str[length] = 0x08;	++length; p+=2;	}
			else if (p[1] == 'f')  { str[length] = 0x0C;	++length; p+=2;	}
			else if (p[1] == 'n')  { str[length] = 0x0A;	++length; p+=2;	}
			else if (p[1] == 'r')  { str[length] = 0x0D;	++length; p+=2;	}
			else if (p[1] == 't')  { str[length] = 0x09;	++length; p+=2;	}
			else if (p[1] == 'v')  { str[length] = 0x0B;	++length; p+=2;	}
			else if (p[1] == '\'') { str[length] = 0x27;	++length; p+=2;	}
			else if (p[1] == '\"') { str[length] = 0x22;	++length; p+=2;	}
			else if (p[1] == '\\') { str[length] = 0x5C;	++length; p+=2;	}
			else if (p[1] == '?')  { str[length] = 0x3F;	++length; p+=2;	}
			else if (p[1] == 'x')  { 
				if (isxdigit(p[2]) && isxdigit(p[3])) { 
					str[length] = (uchar)(0x10*hexachar2num(p[2]) + hexachar2num(p[3])); 
					++length;	p+=4; 
				}
				else if (isxdigit(p[2])) {
					str[length] = (uchar)(hexachar2num(p[2]));
					++length;	p+=3; 
				}
				else {
					str[length] = 0x00;
					++length;	p+=2; 
				}
			}
			else if (isodigit(p[1]))  { 
				if (isodigit(p[2]) && isodigit(p[3])) { 
					str[length] = (uchar)(8*8*hexachar2num(p[1]) + 8*hexachar2num(p[2]) + hexachar2num(p[3])); 
					++length;	p+=4; 
				}
				else if (isodigit(p[2])) {
					str[length] = (uchar)(8*hexachar2num(p[1]) + hexachar2num(p[2]));
					++length;	p+=3; 
				}
				else {
					str[length] = hexachar2num(p[1]);
					++length;	p+=2; 
				}
			}
			else if (p[1]) { str[length] = p[1];	++length; p+=2; }
			else break;
		}
		else               { str[length] = p[0];	++length; p++; }
	}
	return length;
}

int escseq2hexachar (const char *escseq, char *hexachar, int hexachar_len)
{
	char *str = new char[hexachar_len];
	
	int n = escseq2str (escseq, str, hexachar_len);
	n = str2hexachar (str, n, hexachar, hexachar_len);
	
	delete [] str;
	return n;
}

int hexachar2escseq (const char *hexachar, char *escseq, int escseq_len)
{
	char *str = new char[escseq_len];
	
	int n = hexachar2str (hexachar, str, escseq_len);
	n = str2escseq (str, n, escseq, escseq_len);

	delete [] str;
	return n;
}


#pragma once
#include <windows.h>
#include "CommPort.h"

enum eFlowControl {
	FC_NONE = 0, 
	FC_XONXOFF, 
	FC_RTSCTS, 
	FC_DTRDSR,
	FC_FULLHARDWARE, 
};

class CSerialCOM : public CCommPort
{
public:
	CSerialCOM (const char *portName, int baudRate, char dataBits = 8, char parity = NOPARITY, char stopBits = ONESTOPBIT, eFlowControl flowControl = FC_NONE, DWORD timeout = 100);
	virtual ~CSerialCOM ();

	virtual bool Open ();
	virtual bool Connect ();
	virtual bool Close ();

	virtual int  Send (const char *msg, int len);
	virtual int  Recv (      char *msg, int len);
	virtual void Purge ();

	virtual bool ConnectionCheck ();
	virtual bool IsConnected () { return _comHandle != INVALID_HANDLE_VALUE; }
	virtual int CountRx ();

public:
	bool SetTimeout (int readTimeout, int writeTimeout, int readIntervalTimeout);
	void EscapeCommFunc   (DWORD func) { if (_comHandle != INVALID_HANDLE_VALUE) EscapeCommFunction (_comHandle, func); }
	void GetCommModemStat (DWORD &stat) { if (_comHandle != INVALID_HANDLE_VALUE) GetCommModemStatus (_comHandle, &stat); else stat = 0; }

private:
	HANDLE _comHandle; 

	char _portName[32+1];
	int  _baudRate;
	char _dataBits;
	char _stopBits;
	char _parity;
	char _flowControl;
	DWORD _timeout;

private:
	const char *CommErrorString (DWORD comError);
};

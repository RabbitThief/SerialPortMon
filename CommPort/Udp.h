#pragma once 
#include "CommPort.h"

class CUdp : public CEthernetPort {
public:
	CUdp (DWORD localIp, DWORD remoteIp, WORD port, DWORD timeout) 
		: CEthernetPort (localIp, remoteIp, port, timeout), _recvIpAddr(0), _recvPort(0) { }
	virtual ~CUdp () { if (_sockfd != INVALID_SOCKET) Close (); }

	virtual bool Open ();
	virtual bool Connect ();
	virtual bool Close ();

	virtual int  Send (const char *msg, int len);
	virtual int  Recv (      char *msg, int len);

public:
	DWORD _recvIpAddr;
	WORD  _recvPort;
};

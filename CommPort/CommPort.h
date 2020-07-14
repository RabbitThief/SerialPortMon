#pragma once 
#include <WinSock2.h>
#include "SockCommon.h"

class CCommPort 
{
public:
	virtual bool Open () = 0;
	virtual bool Close () = 0;
	virtual bool Connect () = 0;

	virtual int  Send (const char *msg, int len) = 0;
	virtual int  Recv (      char *msg, int len) = 0;
	virtual void Purge () { }

	virtual bool IsConnected () = 0;
	virtual int  CountRx () = 0;
};


class CEthernetPort : public CCommPort {
protected:
	DWORD  _localIp;
	DWORD  _remoteIp;
	WORD   _port;
	DWORD  _timeout;
	SOCKET _sockfd;
	bool   _connected;

public:
	CEthernetPort (DWORD localIp, DWORD remoteIp, WORD port, DWORD timeout) : 
		_localIp(localIp), _remoteIp(remoteIp), _port(port), _timeout(timeout), _sockfd(INVALID_SOCKET), 
		_connected(false) { }

	virtual bool IsConnected () { return (_sockfd != INVALID_SOCKET) && _connected; }
	virtual int  CountRx ()     { return (_sockfd != INVALID_SOCKET) ? SocketCountRx (_sockfd) : -1;  }
};


inline const char *pGetCommStr (int mode)
{
	switch (mode) {
	case 0: return "UDP";
	case 1: return "TCP Server";
	case 2: return "TCP Client";
	case 3: return "Serial(COM)";
	default: return "Unknown";
	}
}

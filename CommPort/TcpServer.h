#pragma once 
#include "CommPort.h"

class CTcpServer : public CEthernetPort {
public:
	CTcpServer (DWORD localIp, WORD port, DWORD timeout) 
		: CEthernetPort (localIp, 0, port, timeout) { }
	virtual ~CTcpServer () { if (_sockfd != INVALID_SOCKET) Close (); }

	virtual bool Open ();
	virtual bool Connect ();
	virtual bool Close ();

	virtual int  Send (const char *msg, int len);
	virtual int  Recv (      char *msg, int len);

	DWORD GetRemoteIP (WORD *port = NULL);
};

#pragma once 
#include "CommPort.h"

class CTcpClient : public CEthernetPort {
public:
	CTcpClient (DWORD remoteIp, WORD port, DWORD timeout) 
		: CEthernetPort (0, remoteIp, port, timeout) { }
	virtual ~CTcpClient () { if (_sockfd != INVALID_SOCKET) Close (); }

	virtual bool Open ();
	virtual bool Connect ();
	virtual bool Close ();

	virtual int  Send (const char *msg, int len);
	virtual int  Recv (      char *msg, int len);

	DWORD GetRemoteIP (WORD *port = NULL);
};

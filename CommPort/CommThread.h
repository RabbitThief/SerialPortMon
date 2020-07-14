#pragma once 
#include "Udp.h"
#include "LockBlock.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "SerialCOM.h"

class CSerialPortMonDlg;

class CCommThread
{
public:
	CCommThread (CCommPort *comm);
	virtual ~CCommThread ();

private:
	void RecvMessage ();
	int ThreadFunc ();

	CLock _lock;
	CCommPort *_comm;
	char *_recvBuff;

	bool _recvCheck;
	bool _stopRequest;
	CWinThread *_pThread;
	CSerialPortMonDlg *_mainDlg;

	friend UINT _ThreadFunc (LPVOID pParam);
};
	

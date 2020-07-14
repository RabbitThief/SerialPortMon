#include "stdafx.h"
#include <assert.h>
#include "Common.h"
#include "CommThread.h"
#include "../resource.h"
#include "../SerialPortMonDlg.h"
#include "../AppIni.h"

UINT _ThreadFunc (LPVOID pParam)
{
	int ret = ((CCommThread *)pParam)->ThreadFunc();
	
	AfxEndThread (ret);
	return ret;
}

CCommThread::CCommThread (CCommPort *comm) : 
	_pThread (NULL), _recvCheck(false), _stopRequest(false), _comm(comm)
{
	/*
	SYSTEM_INFO info;  
	GetSystemInfo(&info);  

	if ((int)info.dwNumberOfProcessors <= _cpuID) {
		_cpuID %= info.dwNumberOfProcessors;

		TRACE ("Warning: CPU �ھ� ������ thread�� �Ҵ��ϱ⿡ �����մϴ�.\n");
	}
	*/

	_stopRequest = false;
	#define MAX_RECV_BUFF 10000
	_recvBuff = new char [MAX_RECV_BUFF+10]; // ���ſ� ���۸� �Ҵ�

	// ���ŵ� �����͸� ���� ���� ��ȭ���� ������
	_mainDlg = dynamic_cast<CSerialPortMonDlg *>(AfxGetApp()->m_pMainWnd);
	assert (_mainDlg);

	_recvCheck = dynamic_cast<CSerialCOM *>(_comm) ? true : false;

	_pThread = AfxBeginThread (_ThreadFunc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	assert (_pThread);

	_pThread->m_bAutoDelete = FALSE;
	_pThread->ResumeThread();
}

CCommThread::~CCommThread ()
{
	DWORD timeout  = 1000; //INFINITE;
	HANDLE hThread = _pThread->m_hThread;

	DWORD dwExitCode = 0;
	::GetExitCodeThread (_pThread->m_hThread, &dwExitCode);
	// �����尡 ���� ��� �ִٸ� dwExitCode == STILL_ACTIVE �� ��
	if (dwExitCode == STILL_ACTIVE) {
		_stopRequest = true;

		if (::WaitForSingleObject (hThread, timeout) == WAIT_OBJECT_0) {
			// �����尡 ���� ����Ǿ���.
		}
		else {	// WAIT_TIMEOUT: Wait timeout
			// timeout���� ��ٷ����� �����尡 ���� ���̴�. 
			// �����带 ���� ������Ѿ� �Ѵ�.
			::TerminateThread (hThread, 0);
		}
	}

	// delete���� �ڵ��� �ݾ��ش�
	//if (hThread != INVALID_HANDLE_VALUE) {
	//	::CloseHandle (hThread);
	//}
	delete _pThread;
	delete [] _recvBuff;
}

int CCommThread::ThreadFunc()
{
	// thread���� _comm ��ü�� �����͸� ����ϰ� �ֱ⶧����
	// _comm ��ü�� thread���� ���� delete �Ǹ� ���� �ȵȴ�.

	while (!_stopRequest) {
		if (_comm->IsConnected ()) {
			int ret = (_recvCheck) ? _comm->CountRx () : 1;

			if (ret > 0) {
				RecvMessage ();
			}
			else if (ret == 0) {
				Sleep (10);
			}
			else {
				// ��ſ� ���� �߻�, �ַ� ������ ������ ���
				_comm->Close ();
			}
		}
		else {
			_comm->Connect ();
			Sleep (10);
		}
	}
	return 0;
}

void CCommThread::RecvMessage ()
{
	int nRecv = _comm->Recv (_recvBuff, MAX_RECV_BUFF);

	if (nRecv > 0) {
		_recvBuff[nRecv] = '\0';

		if (g_ini.options.ASCII_7BITS) ToAscii7bits (_recvBuff, nRecv);

		if (g_ini.options.MESSAGE_DISPLAY_RECV) {
			_mainDlg->LogMessage (_recvBuff, nRecv, false);
		}
	}
}
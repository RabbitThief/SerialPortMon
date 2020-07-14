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

		TRACE ("Warning: CPU 코어 갯수가 thread를 할당하기에 부족합니다.\n");
	}
	*/

	_stopRequest = false;
	#define MAX_RECV_BUFF 10000
	_recvBuff = new char [MAX_RECV_BUFF+10]; // 수신용 버퍼를 할당

	// 수신된 데이터를 보낼 매인 대화상자 얻어오기
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
	// 쓰레드가 아직 살아 있다면 dwExitCode == STILL_ACTIVE 가 됨
	if (dwExitCode == STILL_ACTIVE) {
		_stopRequest = true;

		if (::WaitForSingleObject (hThread, timeout) == WAIT_OBJECT_0) {
			// 쓰레드가 정상 종료되었다.
		}
		else {	// WAIT_TIMEOUT: Wait timeout
			// timeout동안 기다렸지만 쓰레드가 수행 중이다. 
			// 스레드를 직접 종료시켜야 한다.
			::TerminateThread (hThread, 0);
		}
	}

	// delete에서 핸들을 닫아준다
	//if (hThread != INVALID_HANDLE_VALUE) {
	//	::CloseHandle (hThread);
	//}
	delete _pThread;
	delete [] _recvBuff;
}

int CCommThread::ThreadFunc()
{
	// thread에서 _comm 객체의 포인터를 사용하고 있기때문에
	// _comm 객체는 thread보다 먼저 delete 되면 절대 안된다.

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
				// 통신에 오류 발생, 주로 연결이 끊어진 경우
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
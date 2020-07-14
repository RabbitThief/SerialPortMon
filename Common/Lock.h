// �����尣 ����ȭ�� ���� ������� �Ӱ迵��(critical section)�� lock/unlock ��� ����
//
// �Ӱ迵���̶� �����ڿ��� ������ �����ϴ� �ڵ��� ������ ����Ų��. 
// �̴� �Ʒ� �� �Լ��� ����(Lock ())�ϰ� ��(Unlock ())����.
// CLock�� ������ thread�� �����ϹǷ� �ݵ�� ���������� �����Ѵ�.

#pragma once
#include <windows.h>

class CLock  
{
private:
	CRITICAL_SECTION _cs;

public:
	CLock() { InitializeCriticalSection(&_cs);	}
   	~CLock() { DeleteCriticalSection(&_cs); }

	// �����ڿ��� ��� �ٸ� thread�� �����ڿ��� �������� ���ϵ��� �����Ѵ�.
	// �̹� �ٸ� thread�� ���� �����ڿ��� ������ ���, �����ڿ��� ������ ������ ����Ѵ�.
	inline void Lock () { EnterCriticalSection (&_cs); }
	
	// �����ڿ��� �����Ѵ�.
	inline void Unlock () { LeaveCriticalSection (&_cs); }
};

/* Example
// ������ ���� ����Ѵ�.

CLock cs;

�Լ� ()
{
	cs.Lock ();
	// ���� �ڿ��� ���⼭ �����Ѵ�.
	cs.Unlock ();
}

*/
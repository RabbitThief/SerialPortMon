// 쓰레드간 동기화를 위한 방법으로 임계영역(critical section)의 lock/unlock 기능 제공
//
// 임계영역이란 공유자원의 독점을 보장하는 코드의 영역을 가리킨다. 
// 이는 아래 두 함수로 시작(Lock ())하고 끝(Unlock ())낸다.
// CLock은 복수의 thread가 참조하므로 반드시 전역변수로 선언한다.

#pragma once
#include <windows.h>

class CLock  
{
private:
	CRITICAL_SECTION _cs;

public:
	CLock() { InitializeCriticalSection(&_cs);	}
   	~CLock() { DeleteCriticalSection(&_cs); }

	// 공유자원을 잠궈 다른 thread가 공유자원에 접근하지 못하도록 차단한다.
	// 이미 다른 thread에 의해 공유자원이 점유된 경우, 공유자원이 해제될 때까지 대기한다.
	inline void Lock () { EnterCriticalSection (&_cs); }
	
	// 공유자원을 해제한다.
	inline void Unlock () { LeaveCriticalSection (&_cs); }
};

/* Example
// 다음과 같이 사용한다.

CLock cs;

함수 ()
{
	cs.Lock ();
	// 공유 자원을 여기서 독점한다.
	cs.Unlock ();
}

*/
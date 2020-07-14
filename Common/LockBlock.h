
// 블럭으로 설정된 임계영역(critical section)을 잠근다.

#pragma once

#include "Lock.h"

class CLockBlock  
{
private:
	CLock &_lock;

public:
	CLockBlock(CLock &lock) :_lock (lock) { _lock.Lock (); }
	~CLockBlock() { _lock.Unlock (); }
};

#define LOCK_BLOCK(x)	CLockBlock _lock_block_(x)

/* Example
// 다음과 같이 사용한다.

CLock lock;

함수 ()
{
	....
	{
		LOCK_BLOCK (lock);
		// 공유 자원을 여기서 독점한다.
	}
}

*/
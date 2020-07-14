
// ������ ������ �Ӱ迵��(critical section)�� ��ٴ�.

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
// ������ ���� ����Ѵ�.

CLock lock;

�Լ� ()
{
	....
	{
		LOCK_BLOCK (lock);
		// ���� �ڿ��� ���⼭ �����Ѵ�.
	}
}

*/
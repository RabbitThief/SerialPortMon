
// Multimedia timer를 사용하기 위한 함수들 모음

#pragma once 
#pragma comment (lib,"winmm.lib")

typedef unsigned long DWORD;
typedef unsigned int  UINT;


class CMultimediaTimer {
private:
	UINT _timerRes;
	DWORD _timerId;

public:
	CMultimediaTimer ();
	~CMultimediaTimer ();

	virtual void OnTimer (UINT timerId, UINT msg) { }

	void Start (UINT executionPeriod);
	void Stop ();
	bool IsRunning () { return _timerId != -1; }

	static DWORD GetTime ();
	static void Sleep (DWORD ms);

public:
	UINT _period;	// ms
};

#include "stdafx.h"
#include <windows.h>
#include <Mmsystem.h>
#include <assert.h>
#include "MultimediaTimer.h"

CMultimediaTimer::CMultimediaTimer () :
	_timerRes (0), _timerId (-1)
{
	TIMECAPS tc;
	const int TARGET_RESOLUTION	 = 1;	// 1-millisecond target resolution

	timeGetDevCaps(&tc, sizeof(TIMECAPS));
	_timerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
	timeBeginPeriod(_timerRes); 
}

CMultimediaTimer::~CMultimediaTimer ()
{
	Stop ();

	if (0 < _timerRes) {
		timeEndPeriod (_timerRes);
	}
}

static void CALLBACK OneShotTimer(UINT _timerId, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2) 
{
	CMultimediaTimer *pThis = (CMultimediaTimer *)dwUser;
	pThis->OnTimer (_timerId, msg);
} 

void CMultimediaTimer::Start (UINT executionPeriod)
{
	assert ((int)_timerId < 0);
	_period = executionPeriod;
	_timerId = timeSetEvent (executionPeriod, _timerRes, OneShotTimer, (DWORD)this, TIME_PERIODIC);
}

void CMultimediaTimer::Stop ()
{
	if (0 <=_timerId) {
		timeKillEvent (_timerId);  // cancel the event
		_timerId = -1;
	}
}

DWORD CMultimediaTimer::GetTime ()
{
	return timeGetTime(); 
}

void CMultimediaTimer::Sleep (DWORD ms)
{
	::Sleep (ms);
}


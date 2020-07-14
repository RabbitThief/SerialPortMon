#pragma once
#include "log.h"
#include <atltrace.h>


class CExecCounter {
private:
	char _file[256+1];
	char _name[256+1];
	int _line;
	unsigned int _count;
	double _time;
	double _maxTime;

	friend class CPerformTime;

public:
	CExecCounter (const char *file, int line, const char *name) : 
		_line(line), _count(0), _time(0), _maxTime(0) 
	{ 
		strncpy (_file, file, 256); _file[256] = 0;
		strncpy (_name, name, 256); _name[256] = 0;
	}

	~CExecCounter () 
	{
		_time = (0 < _count) ? (double)_time/_count : 0.;
		char *unit = "us";
		if(1000. < _time) { 
			_time /= 1000.; 
			_maxTime /= 1000.;
			unit = "ms"; 
			if(1000. < _time) { 
				_time /= 1000.; 
				_maxTime /= 1000.;
				unit = "s"; 
			}
		}
		ATLTRACE("Performance of '%s' (%s; %d): run count = %d, max time = %g%s, average time = %g%s\n", _name, _file, _line, _count, _maxTime, unit, _time, unit);
		//log ("Performance of '%s' : run count = %d, avg time = %g%s\n", _name, _count, _time, unit);
	}
};

class CPerformTime {
private:
	LARGE_INTEGER freq, start, end;
	CExecCounter &_ec;

public:
	CPerformTime (CExecCounter &ec) : _ec(ec) {
		QueryPerformanceFrequency (&freq);
		QueryPerformanceCounter (&start);
	}
	~CPerformTime () {
		QueryPerformanceCounter(&end);
		double dt = (double)(end.QuadPart-start.QuadPart)/freq.QuadPart*1000000;
		_ec._maxTime = max(_ec._maxTime, dt);
		_ec._count += 1;
		_ec._time += dt;
	}
};

#define PERFORM(name)  static CExecCounter ec(__FILE__, __LINE__, name); CPerformTime pt(ec)


inline double GetPrecisionTime ()
{
	LARGE_INTEGER freq, t;

	QueryPerformanceFrequency (&freq);
	QueryPerformanceCounter(&t);
	return (double)t.QuadPart/freq.QuadPart;
}

/*
// 사용 예
for (int i=0; i<1000; ++i) {
	PERFORM("null()");
	Sleep(1);
}
for (int i=0; i<10000; ++i) {
	PERFORM("cos()");
	cos((double)i);
	Sleep(0);
}
for (int i=0; i<10000; ++i) {
	PERFORM("exp(1.5)");
	exp(1.5);
}
for (int i=0; i<10000; ++i) {
	PERFORM("pow(2)");
	pow((double)i, 2);
}

// 결과
// Microsoft Visual Studio의 output 창
Performance of 'pow(2)' (c:\temp\exe\exe\exedlg.cpp; 227): run count = 10000, avg time = 1.76835us
Performance of 'exp(1.5)' (c:\temp\exe\exe\exedlg.cpp; 223): run count = 10000, avg time = 1.76506us
Performance of 'cos()' (c:\temp\exe\exe\exedlg.cpp; 218): run count = 10000, avg time = 2.09317us
Performance of 'null()' (c:\temp\exe\exe\exedlg.cpp; 214): run count = 1000, avg time = 15.6201ms
*/

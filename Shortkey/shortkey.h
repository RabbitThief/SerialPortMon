#pragma once

struct sShortkey {
	int _count;
	DWORD _to;

	char *_str;
	int _strLen;
	int _delay;
	int _repeat;

	sShortkey () : _count(-1), _to(0), _str(NULL), _strLen(0), _delay(10), _repeat(0) { }

	void Start (char *str, int len, int delay, int repeat);
	void Stop ();
	void Step (DWORD t);
	bool IsRun ();
};

#pragma once

inline int BOUND (int value, int lo, int hi)
{
	if (value < lo) value = lo;
	if (value > hi) value = hi;
	return value;
}

inline void ToAscii7bits (char *msg, int len)
{
	for (int i=0; i<len; i++) {
		msg[i] &= 0x7F;
	}
}


#include "stdafx.h"
#include "AppIni.h" 


CAppIni::CAppIni(const char *inipath)
: CIni(inipath)
{
	LoadData();
}

CAppIni::~CAppIni()
{
	SaveData();
}

void CAppIni::LoadData()
{
	TransferAllData (false);
}

void CAppIni::SaveData()
{
	TransferAllData (true);
}

void CAppIni::TransferAllData (bool bSave)
{
	Transfer (bSave, "mainWndSx",			mainWndSx,				0);
	Transfer (bSave, "mainWndSy",			mainWndSy,				0);
	Transfer (bSave, "mainWndDx",			mainWndDx,				800);
	Transfer (bSave, "mainWndDy",			mainWndDy,				400);

	Transfer (bSave, "shortkeyWndSx",		shortkeyWndSx,			200);
	Transfer (bSave, "shortkeyWndSy",		shortkeyWndSy,			300);

	Transfer (bSave, "options",				(DWORD &)options,		0x3);
	Transfer (bSave, "prefix",				prefix,					16,		"");
	Transfer (bSave, "suffix",				suffix,					16,		"");
	Transfer (bSave, "input",				input,					256,	"");
	prefix[16] = '\0';
	suffix[16] = '\0';
	input[256] = '\0';

	for (int i=0; i<8; i++) {
		char messageX[11] = "message1";		messageX[7] += i;
		char msgOptX[11]  = "msgOpt1";		msgOptX[6]  += i;
		char delayX[11]   = "delay1";		delayX[5]   += i;
		char repeatX[11]  = "repeat1";		repeatX[6]  += i;
		char shortKeyX[11]= "shortKey1";	shortKeyX[8]+= i;

		Transfer (bSave, messageX,			message[i],				256,	"");
		Transfer (bSave, msgOptX,			(DWORD &)msgOpt[i],		0);
		Transfer (bSave, delayX,			delay[i],				100);
		Transfer (bSave, repeatX,			repeat[i],				1);
		Transfer (bSave, shortKeyX,			(DWORD &)shortKey[i],	VK_F1+i);
		
		message[i][256] = '\0';
	}
	Transfer (bSave, "shortKey9",			(DWORD &)shortKey[8],	VK_F1+8);	// Stop All key

	Transfer (bSave, "bufferLines",			bufferLines,			1000);

	Transfer (bSave, "comPort",				comPort,				MAX_PATH,	"\\\\.\\COM1");
	Transfer (bSave, "comBps",				comBps,					115200);
	Transfer (bSave, "comDatabits",			comDatabits,			8);
	Transfer (bSave, "comParity",			comParity,				0);
	Transfer (bSave, "comStopbits",			comStopbits,			0);
	Transfer (bSave, "comFlowcontrol",		comFlowcontrol,			0);
	Transfer (bSave, "remoteIpAddr",		remoteIpAddr,			0xC0A8000A);
	Transfer (bSave, "localIpAddr",			localIpAddr,			0x00000000);
	Transfer (bSave, "portNum",				portNum,				5000);
}

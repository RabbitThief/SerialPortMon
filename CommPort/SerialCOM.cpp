#include "stdafx.h"
#include "SerialCOM.h"

static void OnFileAccessError (const char *errHeader)
{
	int errCode = GetLastError();
	LPSTR errString = NULL;

	int size = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, errCode, 0, (LPSTR)&errString, 0, 0); 

	TRACE ("%s (%d):  %s\n", errHeader, errCode, errString);
	// AfxMessageBox (errString, MB_ICONEXCLAMATION);

	LocalFree (errString); 
}

CSerialCOM::CSerialCOM (const char *portName, int baudRate, char dataBits, char parity, char stopBits, eFlowControl flowControl, DWORD timeout) : 
	_comHandle (INVALID_HANDLE_VALUE),
	_baudRate (baudRate), _dataBits (dataBits), _stopBits (stopBits), _parity (parity), _flowControl (flowControl), _timeout(timeout)
{
	strncpy (_portName, portName, 32);
	_portName[32] = 0;
}

CSerialCOM::~CSerialCOM ()
{
	if (_comHandle != INVALID_HANDLE_VALUE) {
		Close ();
	}
}

bool CSerialCOM::Open ()
{
	TRACE ("INFO: CSerialCOM::Open (): %s, %d\n", _portName, (int)_baudRate);
	if (_comHandle != INVALID_HANDLE_VALUE) {
		TRACE ("WARNING: CSerialCOM::Open (): already opened port \n");
		Close ();
	}

	_comHandle = CreateFile (_portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if(_comHandle==INVALID_HANDLE_VALUE){
		OnFileAccessError ("CSerialCOM::CreateFile (): ");
		goto CLOSE_RET;
	}

	SetupComm (_comHandle, 8192, 8192);
	PurgeComm (_comHandle, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	DCB  dcb;
	if (!GetCommState (_comHandle, &dcb)) {	//dcb의 기본값을 받는다.
		OnFileAccessError ("CSerialCOM::GetCommState (): ");
		goto CLOSE_RET;
	}

	// Set the new data
	dcb.BaudRate	= _baudRate;		// Baudrate at which running       
	dcb.ByteSize	= _dataBits;		// Number of bits/byte, 4-8     
	dcb.Parity		= _parity;			// 0-4=None,Odd,Even,Mark,Space    
	dcb.StopBits	= _stopBits;		// 0,1,2 = 1, 1.5, 2               
	dcb.fParity		= _parity ? true : false;	// Enable parity checking          

	// 일단 기본 flow control 옵션을 설정한다.
	// 뒤에서 flow control 옵션에 따라 필요한 항목을 Enable 한다.
	dcb.fOutxCtsFlow	= false;		// CTS(Clear To Send) handshaking on output       
	dcb.fOutxDsrFlow	= false;		// DSR(Data Set Ready) handshaking on output       
	dcb.fDtrControl		= DTR_CONTROL_ENABLE;	// DTR(Data Terminal Ready) Flow control                
	dcb.fRtsControl		= RTS_CONTROL_ENABLE;	// RTS(Ready To Send) Flow control                
	// Xon, Xoff flow control 시 아래 두 항목을 Enable 한다.
	dcb.fOutX			= false;		// Enable output X-ON/X-OFF        
	dcb.fInX			= false;		// Enable input X-ON/X-OFF         

	//Setup the flow control 
	dcb.fDsrSensitivity = false;
	switch (_flowControl) {
	case FC_XONXOFF:	// Software Handshaking 
		dcb.fOutX = true;
		dcb.fInX = true;
		dcb.XonChar = 0x11;			// Tx and Rx X-ON character      
		dcb.XoffChar = 0x13;		// Tx and Rx X-OFF character   
		dcb.XoffLim = 100;			// Transmit X-ON threshold 
		dcb.XonLim = 100;			// Transmit X-OFF threshold    
		break;
	case FC_RTSCTS:
		dcb.fOutxCtsFlow = true;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		break;
	case FC_DTRDSR:
		dcb.fOutxDsrFlow = true;
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
		break;
	case FC_FULLHARDWARE: // Hardware Handshaking
		dcb.fOutxCtsFlow = true; 
		dcb.fOutxDsrFlow = true; 
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE; 
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE; 
		break;
	}

	dcb.fBinary			= true;			// Binary Mode (skip EOF check)    
	dcb.fNull			= false;		// Enable Null stripping           
	dcb.fAbortOnError	= false;		// Abort all reads and writes on Error 
	dcb.fDsrSensitivity = false;		// DSR Sensitivity              
	dcb.fTXContinueOnXoff = true;		// Continue TX when Xoff sent 
    dcb.fErrorChar		= false;		// Enable Err Replacement          
    dcb.ErrorChar		= 0;			// Error replacement char          
    dcb.EofChar			= 0;			// End of Input character          
    dcb.EvtChar			= 0;			// Received Event character        

	if (!SetCommState(_comHandle, &dcb)) {
		OnFileAccessError ("CSerialCOM::SetCommState (): ");
		goto CLOSE_RET;
	}

	SetTimeout (_timeout, _timeout, 1);

	// connection success
	return true;

CLOSE_RET:
	if (_comHandle != INVALID_HANDLE_VALUE) {
		CloseHandle (_comHandle);
	}
	_comHandle = INVALID_HANDLE_VALUE;
	return false;
}

bool CSerialCOM::Connect () 
{ 
	if (_comHandle == INVALID_HANDLE_VALUE) Open ();
	return (_comHandle != INVALID_HANDLE_VALUE); 
}

bool CSerialCOM::Close ()
{
	TRACE ("INFO: CSerialCOM::Close (): %s, %d\n", _portName, (int)_baudRate);
	if (_comHandle != INVALID_HANDLE_VALUE) {
		CloseHandle (_comHandle);
		_comHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

int CSerialCOM::Send (const char *buff, int length)
{
	if(_comHandle==INVALID_HANDLE_VALUE) return -1;

	DWORD writtenBytes = 0;
	if (WriteFile (_comHandle, buff, length, &writtenBytes, NULL)) {
		return writtenBytes;
	}
	OnFileAccessError ("CSerialCOM::Send (): ");
	Close ();
	return -1;
}

int CSerialCOM::Recv (char *buff, int length)
{
	if(_comHandle==INVALID_HANDLE_VALUE) return -1;

	DWORD readBytes = 0;
	if (ReadFile (_comHandle, buff, length, &readBytes, NULL)){
		return readBytes;
	}
	OnFileAccessError ("CSerialCOM::Recv (): ");
	Close ();
	return -1;
}

void CSerialCOM::Purge ()
{
	if (_comHandle == INVALID_HANDLE_VALUE) return;
	
	DWORD  comError = 0;
	COMSTAT comStat;
	if (ClearCommError (_comHandle, &comError, &comStat)) {
		if (comError) {
			TRACE ("ERROR: CSerialCOM::Purge(): %s\n", CommErrorString (comError));
		}
	}
	else {
		OnFileAccessError ("CSerialCOM::ClearCommError (): ");
	}

	PurgeComm (_comHandle, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
/*
	// 확실하게 Rx buffer를 비우자.
	int n = CountRx ();
	if (0 < n) {
		char *buff = new char [n];
		Recv (buff, n);
		delete [] buff;
	}
*/
}

bool CSerialCOM::SetTimeout (int readTimeout, int writeTimeout, int readIntervalTimeout)
{
	COMMTIMEOUTS commTimeout;
	GetCommTimeouts (_comHandle, &commTimeout);

	commTimeout.ReadIntervalTimeout = readIntervalTimeout;
	commTimeout.ReadTotalTimeoutMultiplier = 0;
	commTimeout.ReadTotalTimeoutConstant = readTimeout;
	commTimeout.WriteTotalTimeoutMultiplier = 0;
	commTimeout.WriteTotalTimeoutConstant = writeTimeout;

	if (SetCommTimeouts (_comHandle, &commTimeout)) {
		return true;
	}
	OnFileAccessError ("CSerialCOM::SetCommTimeouts (): ");
	return false;
}

bool CSerialCOM::ConnectionCheck ()
{
	return true;
}

int CSerialCOM::CountRx ()
{
	if (_comHandle == INVALID_HANDLE_VALUE) return -1;

	DWORD  comError = 0;
	COMSTAT comStat;
	if (ClearCommError(_comHandle, &comError, &comStat)) {
		if (comError) {
			TRACE ("ERROR: CSerialCOM::CountRx(): %s\n", CommErrorString (comError));
		}
		return comStat.cbInQue;
	}
	OnFileAccessError ("CSerialCOM::ClearCommError (): ");
	return -1;
}

const char *CSerialCOM::CommErrorString (DWORD comError)
{
	switch(comError) {
	case 0:           return "";
	case CE_BREAK:    return "The hardware detected a break condition.";
	case CE_FRAME:    return "The hardware detected a framing error.";
	case CE_OVERRUN:  return "A character-buffer overrun has occurred. The next character is lost.";
	case CE_RXOVER:   return "An input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character.";
	case CE_RXPARITY: return "The hardware detected a parity error.";
	default:          return "Unknown error code.";
	}
}
 

#include "stdafx.h"
#include "SockCommon.h"

#pragma comment(lib, "Iphlpapi.lib")


void OnSocketError (const char *prefix)
{
	int errCode = WSAGetLastError();
	LPSTR errString = NULL;

	int size = FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, errCode, 0, (LPSTR)&errString, 0, 0); 

	TRACE ("%s %d:  %s\n", prefix, errCode, errString);

	LocalFree (errString); 
}

const char *ip_addr_str(DWORD ip)
{
	static char str[32];
	sprintf (str, "%d.%d.%d.%d", 
		(unsigned int)((ip>>24)&0xFF),
		(unsigned int)((ip>>16)&0xFF),
		(unsigned int)((ip>>8)&0xFF),
		(unsigned int)((ip>>0)&0xFF)
		);
	return str;
}

const char *ip_addr_str(BYTE ip[4])
{
	static char str[32];
	sprintf (str, "%d.%d.%d.%d", 
		(unsigned int)ip[0], 
		(unsigned int)ip[1], 
		(unsigned int)ip[2], 
		(unsigned int)ip[3]
		);
	return str;
}

const char *mac_addr_str(BYTE mac[6])
{
	static char str[64];
	sprintf (str, "%02X:%02X:%02X:%02X:%02X:%02X", 
		(unsigned int)mac[0], 
		(unsigned int)mac[1], 
		(unsigned int)mac[2], 
		(unsigned int)mac[3],
		(unsigned int)mac[4],
		(unsigned int)mac[5]
		);
	return str;
}

bool SocketReady (int sockfd, int timeout)
{
	struct timeval to = {0, timeout*1000};
	fd_set fds;

	FD_ZERO (&fds);
	FD_SET (sockfd, &fds);

	int ready = select(1, NULL, &fds, NULL, &to);	// ready to send
	// int ready = select(1, &fds, NULL, NULL, &to); // ready to receive
	return (0 < ready);
}

DWORD GetLocalIP ()
{
	struct hostent *hsock;
	char host_name[MAX_PATH];

	if (gethostname (host_name, MAX_PATH) != SOCKET_ERROR) {
		hsock = gethostbyname(host_name);
		if (hsock != NULL) {
			return ntohl(*(DWORD *)hsock->h_addr_list[0]);
		}
 	}
	return -1;
}


int SocketCountRx (int sockfd)
{
	//  입력 버퍼에 들어있는 케릭터 수를 카운트한다.
	DWORD count = 0;
	if (ioctlsocket (sockfd, FIONREAD, &count) == SOCKET_ERROR) {	// handle error
		return -1;
	}
	else {
		return count;
	}
}


vector<DWORD> GetHostIPAddress()
{
	// 호스트에서 인터넷에 연결되어 있는 IP 주소를 가져온다.
	// 호스트에 연결된 IP 주소 중에서 default gw 와 동일한 네트워크에 존재하는 IP 주소를 찾아낸다.
	vector<DWORD> ips;

	DWORD size = 0;
	int ret = GetAdaptersInfo( NULL, &size);

	// Allocate memory from sizing information
	IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO *)malloc (size);

	// Get actual adapter information
	ret = GetAdaptersInfo(pAdapterInfo, &size);
	if (ret) {
		free (pAdapterInfo);
		return ips;
	}

	for (IP_ADAPTER_INFO *pAdapt = pAdapterInfo; pAdapt; pAdapt = pAdapt->Next) {
		//if (pAdapt->Type == MIB_IF_TYPE_ETHERNET) {
			DWORD dwGwIp = inet_addr( pAdapt->GatewayList.IpAddress.String );
			TRACE ("Gateway IP Address = %s\n", pAdapt->GatewayList.IpAddress.String);

 			for (PIP_ADDR_STRING pAddrStr = &(pAdapt->IpAddressList); pAddrStr; pAddrStr = pAddrStr->Next ) {
				ips.push_back (inet_addr (pAddrStr->IpAddress.String));

				TRACE ("    Local IP Address = %s\n", pAddrStr->IpAddress.String);
			}
		//}
	}
	free (pAdapterInfo);
	return ips;
}

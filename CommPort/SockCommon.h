#pragma once 
#include <vector>

using namespace std;

extern void OnSocketError (const char *prefix);
extern bool SocketReady (int sockfd, int timeout);
extern DWORD GetLocalIP ();
extern int SocketCountRx (int sockfd);
extern const char *ip_addr_str(DWORD ip);
extern const char *ip_addr_str(BYTE ip[4]);
extern const char *mac_addr_str(BYTE mac[6]);

extern vector<DWORD> GetHostIPAddress();

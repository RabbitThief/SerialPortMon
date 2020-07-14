#include "stdafx.h"
#include "Udp.h"
#include "SockCommon.h"
#include <assert.h>

bool CUdp::Open ()
{
	TRACE ("INFO: CUdp::Open (): %s, %d\n", ip_addr_str(_remoteIp), (int)_port);
	if (_sockfd != INVALID_SOCKET) {
		TRACE ("WARNING: CUdp::Open (): already opened socket \n");
		Close ();
	}

	_sockfd = socket (AF_INET, SOCK_DGRAM, 0);
	if (_sockfd == INVALID_SOCKET) {
		OnSocketError ("ERROR: socket():");
		goto CLOSE_RET;
	}

	if (_remoteIp == INADDR_BROADCAST) {
		// UDP socket을 broadcasting 으로 설정한다.
		int opt = 1; 
		if (setsockopt (_sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
			OnSocketError ("ERROR: setsockopt(SO_BROADCAST):");
			goto CLOSE_RET;
		}
		//if (setsockopt (_sockfd, SOL_SOCKET, SO_DONTROUTE, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
		//	OnSocketError ("ERROR: setsockopt(SO_DONTROUTE):");
		//	goto CLOSE_RET;
		//}
	}

	// socket의 opetion (sending/receiving timeout 값) 설정
	if (setsockopt (_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		OnSocketError ("ERROR: setsockopt(SO_RCVTIMEO):");
		goto CLOSE_RET;
	}
	if (setsockopt (_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		OnSocketError ("ERROR: setsockopt(SO_SNDTIMEO):");
		goto CLOSE_RET;
	}

	struct sockaddr_in server_addr;
	memset (&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(_localIp);
	server_addr.sin_port = htons(_port);

	if (bind (_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		OnSocketError ("ERROR: bind():");
		goto CLOSE_RET;
	}

	// UDP에서는 소켓 핸들을 얻어오면 연결된 것(_connected == true)으로 간주한다.
	_connected = true;
	return true;

CLOSE_RET:
	if (_sockfd != INVALID_SOCKET) {
		closesocket (_sockfd);
	}
	_sockfd = INVALID_SOCKET;
	return false;
}

bool CUdp::Connect ()
{
	_recvIpAddr = 0;
	_recvPort   = 0;

	if (_sockfd == INVALID_SOCKET) Open ();
	return (_sockfd != INVALID_SOCKET) && _connected;
}

bool CUdp::Close ()
{
	TRACE ("INFO: CUdp::Close (): %s, %d\n", ip_addr_str(_remoteIp), (int)_port);
	if (_sockfd != INVALID_SOCKET) {
		closesocket (_sockfd);
		_sockfd = INVALID_SOCKET;
		_connected = false;
		return true;
	}
	_connected = false;
	return false;
}

int CUdp::Send (const char *msg, int len)
{
	if (_sockfd == INVALID_SOCKET) return -1;
	if (!_connected) return -1;

	struct sockaddr_in server_addr;
	memset (&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(_remoteIp);
	server_addr.sin_port = htons(_port);

	int ret = sendto (_sockfd, msg, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret == 0) {
		Close ();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;

		OnSocketError ("ERROR: sendto():");
		Close ();
		return -1;
	}
	return ret;
}

int CUdp::Recv (char *msg, int len)
{
	if (_sockfd == INVALID_SOCKET) return -1;
	if (!_connected) return -1;

	struct sockaddr_in client_addr;
	int client_addr_size = sizeof( client_addr);

	int ret = recvfrom (_sockfd, msg, len, 0, (struct sockaddr*)&client_addr, &client_addr_size); 
	if (ret == 0) {
		Close ();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;

		OnSocketError ("ERROR: recvfrom():");
		Close ();
		return -1;
	}
	_recvIpAddr = ntohl(client_addr.sin_addr.s_addr);
	_recvPort   = ntohs(client_addr.sin_port);

	return ret;
}

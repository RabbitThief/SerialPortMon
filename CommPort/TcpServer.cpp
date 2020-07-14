#include "stdafx.h"
#include "typedef.h"
#include "SockCommon.h"
#include "TcpServer.h"
#include <assert.h>
#include <mstcpip.h>

bool CTcpServer::Open()
{
	TRACE ("INFO: CTcpServer::Open (): %s, %d\n", ip_addr_str(_remoteIp), (int)_port);
	if (_sockfd != INVALID_SOCKET) {
		TRACE ("WARNING: CTcpServer::Open (): already opened socket \n");
		Close ();
	}

	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == INVALID_SOCKET) {
		OnSocketError ("ERROR: socket():");
		goto CLOSE_RET;
	}

	// 소켓에서 사용하던 Port No를 재사용 가능하도록 한다.
	// listen하던 소켓을 닫은 후 새로 연 경우 한동안 bind ()가 안되는 현상을 막기위해 설정한다.
	bool opt_reuseaddr = true;
	if (setsockopt (_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt_reuseaddr, sizeof(opt_reuseaddr)) == SOCKET_ERROR) {
		OnSocketError ("ERROR: setsockopt(opt_reuseaddr):");
		goto CLOSE_RET;
	}

	struct sockaddr_in server_addr;
	memset (&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(_localIp);
	server_addr.sin_port = htons(_port);

	if (bind (_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==SOCKET_ERROR) {
		OnSocketError ("ERROR: bind():");
		goto CLOSE_RET;
	}

	if (listen (_sockfd, 1) == SOCKET_ERROR) {
		OnSocketError ("ERROR: listen():");
		goto CLOSE_RET;
	}

	_connected = false;
	return true;

CLOSE_RET:
	if (_sockfd != INVALID_SOCKET) {
		closesocket (_sockfd);
	}
	_sockfd = INVALID_SOCKET;
	return false;
}

bool CTcpServer::Close ()
{
	TRACE ("INFO: CTcpServer::Close (): %s, %d\n", ip_addr_str(_remoteIp), (int)_port);
	if (_sockfd != INVALID_SOCKET) {
		shutdown(_sockfd, SD_BOTH);
		closesocket (_sockfd);
		_sockfd = INVALID_SOCKET;
		_connected = false;
		return true;
	}
	_connected = false;
	return false;
}

bool CTcpServer::Connect ()
{
	if (_sockfd == INVALID_SOCKET) Open ();
	if (_connected) return true;

	// Set the socket to non-blocking mode.
	int nonblocking = 1;
	if (ioctlsocket (_sockfd, FIONBIO, (ulong *)&nonblocking) == SOCKET_ERROR) {
		OnSocketError ("ERROR: ioctlsocket(FIONBIO):");
		goto CLOSE_RET;
	}

	struct sockaddr_in client_addr;
	int clilen = sizeof (client_addr);

	int connfd = accept (_sockfd, (struct sockaddr *)&client_addr, &clilen);
	if (connfd == INVALID_SOCKET) {
		int error = WSAGetLastError ();
		if (error == WSAETIMEDOUT || error == WSAEWOULDBLOCK) {
			// 아직 연결하지 못함
			// socket이 사용가능할 때까지 기다린다.
			SocketReady (_sockfd, _timeout);
			return false;
		}
		else {
			OnSocketError ("ERROR: accept():");
			goto CLOSE_RET;
		}
	}
	else {
		closesocket (_sockfd);
		_sockfd = connfd;

		DWORD dwRet = 0;
		struct tcp_keepalive ka;
		ka.onoff = 1;
		ka.keepalivetime = 1000;	
		ka.keepaliveinterval = 1000;
		if (WSAIoctl (_sockfd, SIO_KEEPALIVE_VALS, &ka, sizeof(ka),NULL, 0, &dwRet, NULL, NULL) == SOCKET_ERROR) {
			OnSocketError ("ERROR: WSAIoctl(SIO_KEEPALIVE_VALS):");
			goto CLOSE_RET;
		}

		// Set the socket to blocking mode.
		int nonblocking = 0;
		if (ioctlsocket (_sockfd, FIONBIO, (ulong *)&nonblocking) == SOCKET_ERROR) {
			OnSocketError ("ERROR: ioctlsocket(FIONBIO):");
			goto CLOSE_RET;
		}

		// Set the status for the keepalive option 
		int optval = 1;
		if (setsockopt (_sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval)) == SOCKET_ERROR) {
			OnSocketError ("ERROR: setsockopt(SO_KEEPALIVE):");
			goto CLOSE_RET;
		}

		// socket의 opetion (sending/receiving timeout 값) 설정
		if (setsockopt (_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
			OnSocketError ("ERROR: setsockopt(SO_RCVTIMEO):");
			goto CLOSE_RET;
		}
		if (setsockopt (_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
			OnSocketError ("ERROR: setsockopt(SO_SNDTIMEO):");
			goto CLOSE_RET;
		}

		TRACE ("INFO: CTcpServer::WaitConnection (): Connect to TCP client: %s, %d\n", ip_addr_str(_remoteIp), (int)_port);
		_connected = true;
		return true;

CLOSE_RET:
		if (_sockfd != INVALID_SOCKET) {
			closesocket (_sockfd);
		}
		_sockfd = INVALID_SOCKET;
		return false;
	}
}

int CTcpServer::Send (const char *msg, int len)
{
	if (_sockfd == INVALID_SOCKET) return -1;
	if (!_connected) return -1;

	int ret = send (_sockfd, msg, len, 0);
	if (ret == 0) {
		Close ();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;

		OnSocketError ("ERROR: send():");
		Close ();
		return -1;
	}
	return ret;
}

int CTcpServer::Recv (char *msg, int len)
{
	if (_sockfd == INVALID_SOCKET) return -1;
	if (!_connected) return -1;

	int ret = recv (_sockfd, msg, len, 0);
	if (ret == 0) {
		Close ();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;

		OnSocketError ("ERROR: recv():");
		Close ();
		return -1;
	}
	return ret;
}

DWORD CTcpServer::GetRemoteIP (WORD *port)
{
	struct sockaddr_in client_addr;
	int addrlen = sizeof(client_addr);
	
	int ret = getpeername (_sockfd, (struct sockaddr *)&client_addr, &addrlen);
	if (ret != SOCKET_ERROR) {
		if (port) *port = client_addr.sin_port;
		return ntohl(client_addr.sin_addr.s_addr);
	}
	return 0;
}

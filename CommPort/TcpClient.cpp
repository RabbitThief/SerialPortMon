#include "stdafx.h"
#include "typedef.h"
#include "TcpClient.h"
#include "SockCommon.h"
#include <assert.h>
#include <Mstcpip.h>

bool CTcpClient::Open ()
{
	TRACE ("INFO: CTcpClient::Open (): %s, %d\n", ip_addr_str(_remoteIp), (int)_port);
	if (_sockfd != INVALID_SOCKET) {
		TRACE ("WARNING: CTcpClient::Open (): already opened socket \n");
		Close ();
	}

	_sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (_sockfd == INVALID_SOCKET) {
		OnSocketError ("ERROR: socket():");
		goto CLOSE_RET;
	}

	// Set the socket to non-blocking mode.
	int nonblocking = 1;
	if (ioctlsocket (_sockfd, FIONBIO, (ulong *)&nonblocking) == SOCKET_ERROR) {
		OnSocketError ("ERROR: ioctlsocket(FIONBIO):");
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

bool CTcpClient::Connect ()
{
	if (_sockfd == INVALID_SOCKET) Open ();
	if (_connected) return true;

	// Open에서 nonblocking이 1로 설정되어있다.
	int nonblocking = 1;

	struct sockaddr_in server_addr;
	memset (&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(_remoteIp);
	server_addr.sin_port = htons(_port);

	// non-blocking 모드에서는 connect () 함수는 WSAEWOULDBLOCK 에러코드를 세팅하고 실패(-1)을 리턴한다.
	int ret = connect (_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR) {
		int error = WSAGetLastError ();
		if (nonblocking && (error == WSAEINVAL || error == WSAEWOULDBLOCK)) {
			// socket이 사용가능할 때까지 기다린다.
			if (SocketReady (_sockfd, _timeout)) {
				// Connected
			}
			else {
				//TRACE ("WARNING: CTcpClient::Connect (): Can't connect to TCP server: %s, %d\n", ip_addr_str(_remoteIp), (int)_port);
				return false;
			}
		}
		else if (nonblocking && (error == WSAEALREADY || error == WSAEISCONN)) { // 이미 연결된 소켓
			// Connected
		}
		else {
			OnSocketError ("ERROR: connect():");
			goto CLOSE_RET;
		}
	}
	else { 
		// Connected
	}

	DWORD dwRet = 0;
	struct tcp_keepalive ka;
	ka.onoff = 1;
	ka.keepalivetime = 1000;	
	ka.keepaliveinterval = 1000;
	if (WSAIoctl (_sockfd, SIO_KEEPALIVE_VALS, &ka, sizeof(ka),NULL, 0, &dwRet, NULL, NULL) == SOCKET_ERROR) {
		OnSocketError ("ERROR: WSAIoctl(SIO_KEEPALIVE_VALS):");
		goto CLOSE_RET;
	}

	// 연결되면 blocking 모드로 설정하고 send/recv 시에 timeout 값을 설정하여 무한히 대기하는것을 방지한다.
	nonblocking = 0;
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

	TRACE ("INFO: CTcpClient::Connect (): Connect to TCP server: %s, %d\n", ip_addr_str(_remoteIp), (int)_port);
	_connected = true;
	return true;

CLOSE_RET:
	if (_sockfd != INVALID_SOCKET) {
		closesocket (_sockfd);
	}
	_sockfd = INVALID_SOCKET;
	return false;
}

bool CTcpClient::Close ()
{
	TRACE ("INFO: CTcpClient::Close (): %s, %d\n", ip_addr_str(_remoteIp), (int)_port);
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


int CTcpClient::Send (const char *msg, int len)
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

int CTcpClient::Recv (char *msg, int len)
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

DWORD CTcpClient::GetRemoteIP (WORD *port)
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

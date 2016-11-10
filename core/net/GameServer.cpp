#include "GameServer.h"
#include "..\log\Log.h"

namespace ds  {

	GameServer::GameServer(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port) : _connected(false), _socket(0) , _port(port) , _callback(0) {
		_address = (a << 24) | (b << 16) | (c << 8) | d;
	}

	GameServer::~GameServer() {
		if (_connected) {
			WSACleanup();
		}
	}

	bool GameServer::connect(HTTPCallback* callback) {
		if (!_connected) {
			_callback = callback;
			WSADATA WsaData;
			if (WSAStartup(MAKEWORD(2, 2), &WsaData) != NO_ERROR) {
				return false;
			}

			struct addrinfo hints;
			struct addrinfo *info_result = NULL;

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			// Resolve the server address and port
			int result = getaddrinfo(NULL, "9123", &hints, &info_result);
			if (result != 0) {
				LOGE << "getaddrinfo failed with error: " << result;
				WSACleanup();
				return false;
			}

			// Create a SOCKET for connecting to server
			_socket = socket(info_result->ai_family, info_result->ai_socktype, info_result->ai_protocol);
			if (_socket == INVALID_SOCKET) {
				LOGE << "socket failed with error: " << WSAGetLastError();
				freeaddrinfo(info_result);
				WSACleanup();
				return false;
			}

			// Setup the TCP listening socket
			result = bind(_socket, info_result->ai_addr, (int)info_result->ai_addrlen);
			if (result == SOCKET_ERROR) {
				LOGE << "socket failed with error: " << WSAGetLastError();
				freeaddrinfo(info_result);
				closesocket(_socket);
				WSACleanup();
				return false;
			}

			freeaddrinfo(info_result);

			result = listen(_socket, SOMAXCONN);
			if (result == SOCKET_ERROR) {
				LOGE << "socket failed with error: " << WSAGetLastError();
				closesocket(_socket);
				WSACleanup();
				return false;
			}
		}
		return true;
	}

	const HTTPRequest& GameServer::poll() {
		_request.size = 0;
		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100;
		fd_set fds;
		fd_set out_fds;
		FD_ZERO(&fds);
		FD_ZERO(&out_fds);
		FD_SET(_socket, &fds);
		FD_SET(_socket, &out_fds);
		int ret = select(_socket + 1, &fds, &out_fds, NULL, &tv);
		if (ret != SOCKET_ERROR) {
			if (FD_ISSET(_socket, &fds)) {
				SOCKET client = accept(_socket, NULL, 0);
				if (client != 0) {
					int result = recv(client, _request.data, 1024, 0);
					if (result > 0) {
						_request.data[result] = '\0';
						_request.size = result;
						LOG << "==> Bytes received: " << result;
						LOG << "==> message : '" << _request.data << "'";
						// GET /report
						parseHeader(_request.data);
						// extract method
						// extract path
					}
					if (_callback != 0) {
						_response.data.clear();
						_callback->get(_request, &_response);
						//int sent_bytes = ::send(client, "HTTP/1.0 404 Not found", 22, 0);
						int sent_bytes = ::send(client, _response.data.c_str(), _response.data.size(), 0);
					}
					else {
						int sent_bytes = ::send(client, "HTTP/1.0 404 Not found", 22, 0);
					}
					closesocket(client);
				}
			}
			if (FD_ISSET(_socket, &out_fds)) {
				LOG << "SENDING DATA!!!!!!!";
				int sent_bytes = ::send(_socket, "HTTP/1.0 404 Not found", 22, 0);
			}
		}
		return _request;
	}

	void GameServer::parseHeader(const char* data) {
		// first is method
		const char* tmp = data;
		if (strncmp(tmp, "GET", 3) == 0) {
			_request.method = HTTP_GET;
			tmp += 4;
		}
		else if (strncmp(tmp, "POST", 4) == 0) {
			_request.method = HTTP_GET;
			tmp += 5;
		}
		// next stop is path
		const char* p = tmp;
		int l = 0;
		while (*p != ' ') {
			++p;
			++l;
		}
		strncpy(_request.path, tmp, p - tmp);
		_request.path[l] = '\0';
	}
	
	bool GameServer::send(const char* line) {
		/*
		if (_socket == 0)
			return false;
		int size = strlen(line);
		sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(_address);
		address.sin_port = htons((unsigned short)_port);

		//int sent_bytes = sendto(_socket, line, size, 0, (sockaddr*)&address, sizeof(sockaddr_in));
		int sent_bytes = ::send(_socket, line, size, 0);

		return sent_bytes == size;
		*/
		return true;
	}

	bool GameServer::send(const void* data, int size) {
		//assert(data);
		//assert(size > 0);

		if (_socket == 0)
			return false;

		sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(_address);
		address.sin_port = htons((unsigned short)_port);

		int sent_bytes = sendto(_socket, (const char*)data, size, 0, (sockaddr*)&address, sizeof(sockaddr_in));

		return sent_bytes == size;
	}


	void GameServer::close() {
		if (_socket != 0) {
			closesocket(_socket);
			_socket = 0;
		}
	}

	

}

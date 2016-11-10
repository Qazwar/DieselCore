#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
//#pragma comment( lib, "wsock32.lib" )
#pragma comment (lib, "Ws2_32.lib")
#include <string>

namespace ds {

	enum HTTPMethod {
		HTTP_GET,
		HTTP_POST,
		HTTP_PUT,
		HTTP_DELETE,
		HTTP_UNKNOWN
	};

	struct HTTPRequest {
		HTTPMethod method;
		char path[128];
		char* host;
		char data[1024];
		int size;
	};

	struct HTTPResponse {
		int size;
		//char data[1024];
		std::string data;

	};

	class HTTPCallback {

	public:
		virtual void get(const HTTPRequest& request, HTTPResponse* response) = 0;
	};

	class GameServer {

	public:
		GameServer(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
		~GameServer();
		bool connect(HTTPCallback* callback);
		const HTTPRequest& poll();
		bool send(const void* data, int size);
		bool send(const char* line);
		void close();
	private:
		void parseHeader(const char* data);
		HTTPRequest _request;
		HTTPResponse _response;
		char _data[1024];
		int _size;
		bool _connected;
		int _socket;
		unsigned short _port;
		unsigned int _address;
		HTTPCallback* _callback;
	};

}


#include "Client.h"

#include <iostream>
#include <cassert>

#include "Packet.h"

#define DEFAULT_PORT "23001"
#define DEFAULT_HOST "192.168.1.4"

Client::Client() :
	_id					( -1 ),
	_started			( false ),
	_connect_socket		( INVALID_SOCKET )
{
	load_client_functions();
}

Client::~Client() {
	closesocket(_connect_socket);

	if (_started) {
		WSACleanup();
		if (_recieve_thread.joinable()) {
			_recieve_thread.detach();
		}
	}
}

void Client::c_startup() {
	int r_startup = WSAStartup(MAKEWORD(2, 2), &_wsa_data);
	std::cout << "WsaStartup -- " << r_startup << '\n';
	_started = true;

	addrinfo* result, * ptr, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int r_addrinfo = getaddrinfo(DEFAULT_HOST, DEFAULT_PORT, &hints, &result);
	std::cout << "getaddrinfo -- " << r_addrinfo << '\n';

	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		_connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (_connect_socket == INVALID_SOCKET) {
			std::cout << "Socket Error: " << WSAGetLastError() << '\n';
			continue;
		}

		break;
	}

	assert(ptr);

	int r_connect = connect(_connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (r_connect == SOCKET_ERROR) {
		std::cout << "Connect Error : " << WSAGetLastError() << '\n';
	}
	else {
		_recieve_thread = std::thread(&Client::c_recieve, this);
	}
}

bool Client::c_send(const char* data, int* len) {
	int total = 0;
	int bytes_left = *len;

	while (total < bytes_left) {
		int r_send = send(_connect_socket, data + total, bytes_left, 0);
		std::cout << "send Result: " << r_send << '\n';
		if (r_send == SOCKET_ERROR) {
			std::cout << "Send Error: " << WSAGetLastError() << '\n';
			return false;
		}

		total += r_send;
		bytes_left -= r_send;
	}

	*len = total;

	return true;
}

void Client::c_recieve() {
	char recvbuf[512];
	char* ptr;
	int r_recv = -1, r_send = -1;
	int recvbuflen = 512;

	do {
		std::cout << "Waiting for server..." << '\n';
		r_recv = recv(_connect_socket, recvbuf, recvbuflen - 1, 0);
		if(r_recv > 0) {
			std::cout << "Recieved --- " << r_recv << '\n';
			ptr = recvbuf;
			std::cout << recvbuf << '\n';

			assert(r_recv > 54);
			(this->*_client_functions.at(recvbuf))(static_cast<void*>(recvbuf + 54), r_recv - 54);
		}
		else if(r_recv == 0) {
			std::cout << "Close Connection" << '\n';
		}
		else {
			std::cout << "Recv Error : " << WSAGetLastError() << '\n';
		}
	} while (r_recv > 0);
}

bool Client::c_connected() {
	return _connect_socket == INVALID_SOCKET;
}

void Client::load_client_functions() {
	_client_functions.emplace("set_id", &Client::set_id);
}

void Client::set_id(void* buf, int size) {
	assert(size == sizeof(int));

	memcpy(&_id, buf, sizeof(int));
}

/********************************************************************************************************************************************************/
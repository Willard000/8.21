#include "Server.h"

#include <iostream>
#include <cassert>

#include <string>
#include <iterator>

#include "Packet.h"

#define DEFAULT_PORT "23001"
#define MAX_CLIENTS 12

/********************************************************************************************************************************************************/

void print_error(int val) {
	if (val == SOCKET_ERROR) {
		std::cout << "Error --- " << WSAGetLastError() << '\n';
	}
}

/********************************************************************************************************************************************************/

Server::Server() :
	_listen_socket		( INVALID_SOCKET ),
	_started			( false ),
	_accept				( false )
{}

Server::~Server() {
	closesocket(_listen_socket);

	if (_started) {
		WSACleanup();
	}
}

void Server::s_startup() {
	int r_startup = WSAStartup(MAKEWORD(2, 2), &_wsa_data);
	std::cout << "WSAStartup --- " << r_startup << '\n';
	_started = true;

	addrinfo* result, * ptr, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int r_addrinfo = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
	std::cout << "getaddrinfo --- " << r_addrinfo << '\n';

	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		_listen_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (_listen_socket == INVALID_SOCKET) {
			std::cout << "socket Error --- " << WSAGetLastError() << '\n';
			continue;
		}

		int r_bind = bind(_listen_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		std::cout << "bind --- " << r_bind << '\n';
		if (r_bind == SOCKET_ERROR) {
			print_error(r_bind);
			continue;
		}

		break;
	}

	assert(ptr);

	freeaddrinfo(result);

	int r_listen = listen(_listen_socket, SOMAXCONN);
	std::cout << "listen --- " << r_listen << '\n';
	print_error(r_listen);
}

void Server::s_accept() {
	_accept = true;
	while (_accept) {
		SOCKET client_socket = INVALID_SOCKET;

		std::cout << "Waiting for client..." << '\n';
		client_socket = accept(_listen_socket, nullptr, nullptr);
		if (client_socket == INVALID_SOCKET) {
			std::cout << "Client Accept Error --- " << WSAGetLastError << '\n';
		}
		assert((client_socket != INVALID_SOCKET));
		std::cout << "New Connection" << '\n';

		if (_clients.size() >= MAX_CLIENTS) {
			// reject
			continue;
		}

		auto client = std::make_shared<ServerClient>(client_socket, _clients.size());

		PacketData data("set_id", client->_id);
		int len = data.length();

		s_send(data.c_str(), &len, client);

		_m.lock();
		_clients.push_back(client);
		_m.unlock();

		client->_thread = std::thread(&Server::s_recieve, this, client);
	}
}

void Server::s_decline() {
	_accept = false;
}

void Server::s_recieve(std::shared_ptr<ServerClient> client) {
	char recvbuf[512];
	char* ptr;
	int r_recv = -1, r_send = -1;
	int recvbuflen = 512;

	do {
		r_recv = recv(client->_client_socket, recvbuf, recvbuflen - 1, 0);
		if (r_recv > 0) {
			std::cout << "Recieved --- " << r_recv << '\n';
			ptr = recvbuf;
			std::cout << recvbuf << '\n';

			assert(r_recv > 54);
			(this->*_server_functions.at(recvbuf))(static_cast<void*>(recvbuf - 54), r_recv - 54);
		}
		else if (r_recv == 0) {
			std::cout << "Close Connection" << '\n';
		}
		else {
			std::cout << "Recv Error : " << WSAGetLastError() << '\n';
		}

	} while (r_recv > 0);

	_m.lock();
	auto it = _clients.begin();
	while (it != _clients.end() && (*it)->_id != client->_id) {
		++it;
	}
	if (it != _clients.end()) {
		_clients.erase(it);
	}
	_m.unlock();
}

bool Server::s_send(const char* data, int* len, std::shared_ptr<ServerClient> client) {
	int total = 0;
	int bytes_left = *len;

	while (total < bytes_left) {
		int r_send = send(client->_client_socket, data + total, bytes_left, 0);
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

void Server::load_server_functions() {

}

void Server::new_entity(void* buf, int size) {

}

void Server::move_entity(void* buf, int size) {

}

/********************************************************************************************************************************************************/

ServerClient::ServerClient(SOCKET client_socket, int id) :
	_id(id),
	_client_socket(client_socket)
{}

ServerClient::~ServerClient() {
	closesocket(_client_socket);
	_thread.detach();
}

/********************************************************************************************************************************************************/
#include "Server.h"

#include <iostream>
#include <cassert>

#include <string>
#include <iterator>
#include <filesystem>

#include "Packet.h"

#include "../src/Utility/Clock.h"
#include "../src/Resources/Window.h"
#include "../src/System/ResourceManager.h"

#include <GLFW/glfw3.h>


#define DEFAULT_PORT "23001"
#define MAX_CLIENTS 12

/********************************************************************************************************************************************************/

void print_error(int val) {
	if (val == SOCKET_ERROR) {
		std::cout << "Error --- " << WSAGetLastError() << '\n';
	}
}

/********************************************************************************************************************************************************/

WorldServer::WorldServer() :
	_map_id			( -1 )
{
	if (!glfwInit()) {
		//...
		assert(NULL);
	}

	_environment.set_mode(MODE_SERVER);

	Clock* clock = new Clock;
	_environment.set_clock(clock);

	Window* window = new Window;
	window->hide();
	_environment.set_window(window);

	ResourceManager* resource_manager = new ResourceManager;
	_environment.set_resource_manager(resource_manager);
	resource_manager->load_resources(1, 0, 1, 1, 0);
}

void WorldServer::load() {
	// send map id

	// load map entities to server

	for (auto& p : std::filesystem::directory_iterator("Data\\Map\\Entities")) {
		auto entity = std::make_shared<Entity>();
		entity->load(p.path().string());
		_entities.push_back(entity);
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

	load_server_commands();
	WorldServer::load();
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

		_m.lock();
		_clients.push_back(client);
		_m.unlock();

		client->_thread = std::thread(&Server::s_recieve, this, client);

		PacketData data("set_id", client->_id);
		int len = data.length();

		s_send(data.c_str(), &len, client->_id);
	}
}

void Server::s_decline() {
	_accept = false;
}

void Server::s_recieve(std::shared_ptr<ServerClient> client) {
	char recvbuf[1024];
	char* ptr;
	int r_recv = -1;
	int recvbuflen = 512;
	int bytes_handled = 0;
	int remaining_bytes = 0;
	int packet_length = 0;

	do {
		bytes_handled = 0;
		r_recv = recv(client->_client_socket, recvbuf + remaining_bytes, recvbuflen, 0);
		if (r_recv > 0) {
			std::cout << "Recieved --- " << r_recv << '\n';
			ptr = recvbuf;

			memcpy(&packet_length, ptr, sizeof(int));
			remaining_bytes = r_recv + remaining_bytes;

			while (remaining_bytes >= packet_length) {
				std::cout << ptr + 4 << '\n';

				const char* key = ptr + 4; // skip int header
				if (_server_commands.find(key) == _server_commands.end()) {
					std::cout << "Unknown Server Command : " << key << '\n';
				}
				else {
					(this->*_server_commands.at(ptr + 4))(static_cast<void*>(ptr + 58), packet_length - 58);
				}

				bytes_handled += packet_length;
				remaining_bytes -= packet_length;

				if (remaining_bytes > 4) {
					ptr = recvbuf + bytes_handled;
					memcpy(&packet_length, ptr, sizeof(int));
				}
			}

			if (remaining_bytes > 0) {
				memcpy(recvbuf, ptr, remaining_bytes);
			}
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

bool Server::s_send(const char* data, int* len, int client_id) {
	assert(*len <= 512);

	std::shared_ptr<ServerClient> client = nullptr;
	for(const auto c : _clients) {
		if(c->_id == client_id) {
			client = c;
		}
	}
	if(!client) {
		// client doesnt exist / disconnected
		std::cout << "Client -- " << client_id << " Doesnt exist " << '\n';
		return false;
	}

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

void Server::load_server_commands() {
	_server_commands.emplace("load_world_server", &Server::load_world_server_to_client);
}

// Params: int client_id
void Server::load_world_server_to_client(void* buf, int size) {
	assert(size == sizeof(int));
	int client_id;
	memcpy(&client_id, buf, sizeof(int));

	assert(_clients.size() >= client_id);

	for(const auto e : _entities) {
		auto packet_data_vec = e->packet_data();
		PacketData packet("load_entity");
		for(auto& p : packet_data_vec) {
			packet.add(std::move(p));
		}
		int len = packet.length();
		s_send(packet.c_str(), &len, client_id);
	}
}

void Server::new_entity(void* buf, int size) {

}

void Server::move_entity(void* buf, int size) {

}

/********************************************************************************************************************************************************/

ServerClient::ServerClient(SOCKET client_socket, int id) :
	_id				( id ),
	_client_socket	( client_socket )
{}

ServerClient::~ServerClient() {
	closesocket(_client_socket);
	_thread.detach();
}

/********************************************************************************************************************************************************/
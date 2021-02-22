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

#include "Fmtout.h"

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

void WorldServer::update() {
	for(auto it : _entities) {
		it.second->update();
	}
}

void WorldServer::load() {
	// send map id

	// load map entities to server

	for (auto& p : std::filesystem::directory_iterator("Data\\Map\\Entities")) {
		auto entity = std::make_shared<Entity>();
		entity->load(p.path().string());
		_entities.insert({ entity->get_unique_id(), entity });
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

		fmtout("Waiting for client...");
		client_socket = accept(_listen_socket, nullptr, nullptr);
		if (client_socket == INVALID_SOCKET) {
			fmtout("Client Accept Error --- ", WSAGetLastError);
		}
		assert((client_socket != INVALID_SOCKET));
		fmtout("New Connection");

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
	char recvbuf[2048];
	char* ptr;
	int r_recv = -1;
	int recvbuflen = 1024;
	int bytes_handled = 0;
	int remaining_bytes = 0;
	int packet_length = 0;

	do {
		bytes_handled = 0;
		r_recv = recv(client->_client_socket, recvbuf + remaining_bytes, recvbuflen, 0);
		if (r_recv > 0) {
			dbgout("Receiving...", r_recv);
			ptr = recvbuf;
			remaining_bytes = r_recv + remaining_bytes;

			if (remaining_bytes >= 4) {
				memcpy(&packet_length, ptr, sizeof(int));

				while (remaining_bytes >= packet_length) {
					dbgout("Command --- ", ptr + 4); // command

					const char* key = ptr + 4; // skip int header
					int command_length = strlen(ptr + 4) + 1;
					if (_server_commands.find(key) == _server_commands.end()) {
						dbgout("Unknown Server Command --- ", key);
					}
					else {
						(this->*_server_commands.at(ptr + 4))(static_cast<void*>(ptr + command_length + 4), packet_length - command_length - 4);
					}

					bytes_handled += packet_length;
					remaining_bytes -= packet_length;

					if (remaining_bytes >= 4) {
						ptr = recvbuf + bytes_handled;
						memcpy(&packet_length, ptr, sizeof(int));
					}
				}

			}

			if (remaining_bytes > 0) {
				memcpy(recvbuf, ptr, remaining_bytes);
			}
		}
		else if (r_recv == 0) {
			fmtout("Close Connection");
		}
		else {
			fmtout("Recv Error --- ", WSAGetLastError());
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
	assert(*len <= 1024);

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
		dbgout("Sending...", r_send);
		if (r_send == SOCKET_ERROR) {
			dbgout("Send Error --- ", WSAGetLastError());
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
	_server_commands.emplace("new_entity", &Server::new_entity);
	_server_commands.emplace("set_destination", &Server::set_destination);
}

// Params: int client_id
void Server::load_world_server_to_client(void* buf, int size) {
	assert(size == sizeof(int));
	int client_id;
	memcpy(&client_id, buf, sizeof(int));

	assert(_clients.size() >= client_id);

	for(const auto e : _entities) {
		auto packet_data_vec = e.second->packet_data();
		PacketData packet("load_entity");

		for(auto& p : packet_data_vec) {
			packet.add(std::move(p));
		}
		int len = packet.length();

		s_send(packet.c_str(), &len, client_id);
	}
}

// Params: int client_id, Entity entity
void Server::new_entity(void* buf, int size) {
	assert(size >= sizeof(int));
	int client_id;
	memcpy(&client_id, buf, sizeof(int));

	assert(_clients.size() >= client_id);

	char* ptr = static_cast<char*>(buf);
	ptr = ptr + 4;
	size -= 4;

	std::shared_ptr<Entity> entity = std::make_shared<Entity>();

	int unique_id = entity->get_unique_id();
	entity->load_buffer(static_cast<void*>(ptr), size);
	entity->set_unique_id(unique_id);

	_entities.insert({ entity->get_unique_id(), entity });

	std::cout << "UNIQUE ID: " << entity->get_unique_id() << '\n';
	
	PacketData packet("load_entity");
	auto packet_vector = entity->packet_data();
	for(auto& p : packet_vector) {
		packet.add(std::move(p));
	}

	for(const auto client : _clients) {
		int len = packet.length();
		s_send(packet.c_str(), &len, client->_id);
	}
}

// int client id, int entity_id, vec3 destination
void Server::set_destination(void* buf, int size) {
	// verify bytes
	char* ptr = static_cast<char*>(buf);

	int client_id;
	memcpy(&client_id, ptr, sizeof(int));

	assert(_clients.size() >= client_id);

	int entity_id;
	memcpy(&entity_id, ptr + 4, sizeof(int));

	glm::vec3 destination;
	memcpy(&destination, ptr + 8, sizeof(glm::vec3));

	if(!_entities.count(entity_id)) {
		return;
	}

	_entities.at(entity_id)->get<TransformComponent>()->set_destination(destination);

	PacketData data("set_destination", entity_id, destination);

	for(auto& client : _clients) {
		int len = data.length();
		s_send(data.c_str(), &len, client->_id);
	}
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
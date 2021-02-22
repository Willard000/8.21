#include "Client.h"

#include <iostream>
#include <cassert>

#include "Packet.h"

#include "../src/Entities/Entity.h"
#include "../src/System/Environment.h"
#include "../src/System/ResourceManager.h"

#include "Fmtout.h"

#define DEFAULT_PORT "23001"
#define DEFAULT_HOST "192.168.1.4"

Client::Client() :
	_id					( -1 ),
	_started			( false ),
	_connect_socket		( INVALID_SOCKET )
{
	load_client_commands();
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
	assert(*len <= 1024);

	int total = 0;
	int bytes_left = *len;
	while (total < bytes_left) {
		int r_send = send(_connect_socket, data + total, bytes_left, 0);
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

void Client::c_recieve() {
	char recvbuf[2048];
	char* ptr;
	int r_recv = -1;
	int recvbuflen = 1024;
	int bytes_handled = 0;
	int remaining_bytes = 0;
	int packet_length = 0;

	do {
		bytes_handled = 0;
		r_recv = recv(_connect_socket, recvbuf + remaining_bytes, recvbuflen, 0);
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
					if (_client_commands.find(key) == _client_commands.end()) {
						dbgout("Unknown Client Command --- ", key);
					}
					else {
						(this->*_client_commands.at(ptr + 4))(static_cast<void*>(ptr + command_length + 4), packet_length - command_length - 4);
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

}

bool Client::c_connected() {
	return _connect_socket == INVALID_SOCKET;
}

void Client::s_load_world_server() {
	PacketData packet("load_world_server", _id);
	int len = packet.length();

	c_send(packet.c_str(), &len);
}

void Client::s_new_entity(std::shared_ptr<Entity> entity) {
	PacketData packet("new_entity", _id);

	auto packet_vector = entity->packet_data();
	for(auto& p : packet_vector) {
		packet.add(std::move(p));
	}

	int len = packet.length();
	c_send(packet.c_str(), &len);
}

void Client::load_client_commands() {
	_client_commands.emplace("set_id", &Client::set_id);
	_client_commands.emplace("load_entity", &Client::load_entity);
	_client_commands.emplace("set_destination", &Client::set_destination);
}

void Client::set_id(void* buf, int size) {
	assert(size == sizeof(int));

	memcpy(&_id, buf, sizeof(int));
}

//_unique_id, _id, _type, _model_id, _name, _draw, _destroy

void Client::load_entity(void* buf, int size) {
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();

	entity->load_buffer(buf, size);

	std::cout << "UNIQUE ID" << entity->get_unique_id() << '\n';
	
	Environment::get().get_resource_manager()->add_entity(entity);
}

void Client::set_destination(void* buf, int size) {
	int entity_id;
	memcpy(&entity_id, buf, sizeof(int));

	char* ptr = static_cast<char*>(buf);
	ptr = ptr + 4;

	glm::vec3 destination;
	memcpy(&destination, ptr, sizeof(glm::vec3));

	auto entities = Environment::get().get_resource_manager()->get_entities();

	if(!entities->count(entity_id)) {
		assert(NULL);
	}

	entities->at(entity_id)->get<TransformComponent>()->set_destination(destination);
}

int Client::get_id() {
	return _id;
}

/********************************************************************************************************************************************************/
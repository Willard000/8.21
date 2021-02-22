#ifndef CLIENT_H
#define CLIENT_H

#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#include <thread>
#include <unordered_map>
#include <string>
#include <string_view>

class Client;
class Entity;

typedef void(Client::*ClientCommand)(void* buf, int size);

class Client {
public:
	Client();
	~Client();

	void c_startup();
	bool c_send(const char* data, int* len);
	void c_recieve();
	bool c_connected();
	void c_read(uint8_t* data);

	void load_client_commands();

	void s_load_world_server();
	void s_new_entity(std::shared_ptr<Entity> entity);

	void set_id(void* buf, int size);
	void load_entity(void* buf, int size);
	void set_destination(void* buf, int size);

	int get_id();
private:
	int _id;
	bool _started;

	WSAData _wsa_data;
	SOCKET _connect_socket;

	std::thread _recieve_thread;

	std::unordered_map<std::string, ClientCommand> _client_commands;
};

/********************************************************************************************************************************************************/

#endif
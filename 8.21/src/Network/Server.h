#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "../src/System/Environment.h"
#include "../src/Entities/Entity.h"


/********************************************************************************************************************************************************/

class WorldServer {
public:
	WorldServer();

	void update();

	void load();

protected:
	int _map_id;

	//std::vector<std::shared_ptr<Entity>> _entities;
	std::unordered_map<int, std::shared_ptr<Entity>> _entities;

	Environment _environment;
};

/********************************************************************************************************************************************************/

class ServerClient;
class Server;

typedef void(Server::* ServerCommand)(void* buf, int size);

class Server : public WorldServer{
public:
	Server();
	~Server();

	void s_startup();
	void s_accept();
	void s_decline();
	void s_recieve(std::shared_ptr<ServerClient> client);
	bool s_send(const char* data, int* len, int client_id);

	void load_server_commands();
	void load_world_server_to_client(void* buf, int size);
	void new_entity(void* buf, int size);
	void set_destination(void* buf, int size);
private:
	bool _accept;
	bool _started;

	std::vector<std::shared_ptr<ServerClient>> _clients;

	std::unordered_map<std::string, ServerCommand> _server_commands;

	WSAData _wsa_data;
	SOCKET _listen_socket;

	std::mutex _m;
};

/********************************************************************************************************************************************************/

class ServerClient {
public:
	ServerClient(SOCKET client_socket, int id);
	~ServerClient();
private:
	int _id;

	SOCKET _client_socket;

	std::thread _thread;

	friend class Server;
};

/********************************************************************************************************************************************************/

#endif
#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>

/********************************************************************************************************************************************************/

class ServerClient;
class Server;

typedef void(Server::* ServerFunction)(void* buf, int size);

class Server {
public:
	Server();
	~Server();

	void s_startup();
	void s_accept();
	void s_decline();
	void s_recieve(std::shared_ptr<ServerClient> client);
	bool s_send(const char* data, int* len, std::shared_ptr<ServerClient> client);

	void load_server_functions();
	void new_entity(void* buf, int size);
	void move_entity(void* buf, int size);
private:
	bool _accept;
	bool _started;

	std::vector<std::shared_ptr<ServerClient>> _clients;

	std::unordered_map<std::string, ServerFunction> _server_functions;

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
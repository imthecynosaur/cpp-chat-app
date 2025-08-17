#pragma once

#include <vector>
#include <mutex>
#include <string>

using SOCKET = unsigned __int64;
struct addrinfo;

class Server {
public:
	Server(const char* port);
	~Server();
	void run();

private:
	void resolveServerAddress();
	void createListenSocket();
	void acceptClients();
	void handleClinet(SOCKET clientSocket);
	void broadcastMessage(SOCKET senderSocket, const char* message, int messageLen);


	int bufferLen{ 512 };
	const char* port{};
	addrinfo* serverAddrInfo{ nullptr };

	SOCKET listenSocket;
	std::vector <SOCKET> clientSockets;
	std::mutex clientSocketsMutex;
};
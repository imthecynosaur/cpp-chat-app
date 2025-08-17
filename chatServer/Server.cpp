#include "Server.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <thread>
#include "WinsockException.h"
#include "SocketWrapper.h"


Server::Server(const char* port) :
	port(port), listenSocket(INVALID_SOCKET) {
	resolveServerAddress();
	createListenSocket();
}

Server::~Server() {
	if (listenSocket != INVALID_SOCKET) {
		closesocket(listenSocket);
	}
	if (serverAddrInfo) {
		freeaddrinfo(serverAddrInfo);
	}
}

void Server::run() {
	acceptClients();
}

void Server::resolveServerAddress(){
	addrinfo hints{};
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int result = getaddrinfo(NULL, port, &hints, &serverAddrInfo);
	if (result != 0) {
		throw WinsockException("getAddrInfo failed", result);
	}
	std::cout << "Server address info resloved." << std::endl;
}

void Server::createListenSocket() {
	listenSocket = socket(serverAddrInfo->ai_family, serverAddrInfo->ai_socktype, serverAddrInfo->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		throw WinsockException("socket creation failed", WSAGetLastError());
	}
	if (bind(listenSocket, serverAddrInfo->ai_addr, (int)serverAddrInfo->ai_addrlen) == SOCKET_ERROR) {
		closesocket(listenSocket);
		throw WinsockException("bind failed", WSAGetLastError());
	}
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(listenSocket);
		throw WinsockException("listen failed", WSAGetLastError());
	}
	std::cout << "server is listening on port " << port << " ..." << std::endl;
}

void Server::acceptClients() {
	std::cout << "server is waiting for incoming connections..." << std::endl;

	while (true) {
		SOCKET rawClientSocket = accept(listenSocket, NULL, NULL);
		if (rawClientSocket == INVALID_SOCKET) {
			std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
			continue;
		}

		SocketWrapper clientSocket(rawClientSocket);
		std::cout << "Client connected! socket: " << clientSocket.get() << std::endl;
		{
			std::lock_guard<std::mutex> lock(clientSocketsMutex);
			clientSockets.push_back(clientSocket.get());
		}

		std::thread clientThread(&Server::handleClinet, this, std::move(clientSocket));
		clientThread.detach();
	}
}

void Server::handleClinet(SOCKET clientSocket) {
	std::cout << "Handling communication for client on socket: " << clientSocket << std::endl;
	std::vector<char> recvbuf(bufferLen);
	int result;

	do {
		result = recv(clientSocket, recvbuf.data(), static_cast<int>(recvbuf.size()), 0);
		if (result > 0) {
			broadcastMessage(clientSocket, recvbuf.data(), result);
		}
		else if (result == 0) {
			std::cout << "Client " << clientSocket << " disconnected." << std::endl;
		}
		else {
			std::cerr << "recv failed for client " << clientSocket << "with error: " << WSAGetLastError() << std::endl;
		}
	} while (result > 0);

	{
		std::lock_guard<std::mutex> lock(clientSocketsMutex);
		clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), clientSocket), clientSockets.end());
	}

	closesocket(clientSocket);
	std::cout << "Finished handling client " << clientSocket << std::endl;
}

void Server::broadcastMessage(SOCKET senderSocket, const char* message, int messageLen) {
	std::string senderID = std::to_string(senderSocket);
	std::string finalMessage = "[client " + senderID + "]: " + std::string(message, messageLen);

	std::lock_guard<std::mutex> lock(clientSocketsMutex);

	std::cout << "[DEBUG] Broadcasting message: " << finalMessage << std::endl;

	for (SOCKET otherClinetSocket : clientSockets) {
		if (otherClinetSocket != senderSocket) {
			send(otherClinetSocket, finalMessage.c_str(), static_cast<int>(finalMessage.length()), 0);
		}
	}
}
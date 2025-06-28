#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#include "WinsockException.h"
#include "SocketWrapper.h"
#include "WSAWrapper.h"

std::vector<SOCKET> clientSockets;
std::mutex clientSocketsMutex;

constexpr const char* DEFAULT_PORT = "8080";
constexpr int BUFFER_LEN = 512;


void resolveAddr(const char* port, addrinfo*& outAddrInfo);

SocketWrapper createListenSocket(addrinfo* serveraddr);

void handleClient(SocketWrapper clientSocket);

struct AddrInfoDeletr {
	void operator() (addrinfo* ptr) const{
		if (ptr) {
			std::cout << "AddrInfoDeleter being called" << std::endl;
			freeaddrinfo(ptr);
		}
	}
};


int main() {
	try {
		WSAWrapper wsa;
		addrinfo* rawAddrPtr = nullptr;
		resolveAddr(DEFAULT_PORT, rawAddrPtr);
		std::unique_ptr<addrinfo, AddrInfoDeletr> serveraddr(rawAddrPtr);
		SocketWrapper listenSocket = createListenSocket(serveraddr.get());

		std::cout << "-------------------------" << std::endl;
		std::cout << "Waiting for incoming connections..." << std::endl;
		while (true) {
			SocketWrapper clientSocket(accept(listenSocket, NULL, NULL));
			if (clientSocket == INVALID_SOCKET) {
				std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
				continue;
			}
			std::cout << "Client Connected! Socket ID: " << clientSocket << std::endl;

			{
				std::lock_guard<std::mutex> lock(clientSocketsMutex);
				clientSockets.push_back(clientSocket.get());
			}

			std::thread clientThread(handleClient, std::move(clientSocket));
			clientThread.detach();
		}
		
	}
	catch(const std::exception& e){
		std::cerr << "Error: " << e.what() << std::endl;
		std::cout << "Press Enter to exit." << std::endl;
		std::cin.get();
		return 1;
	}

	std::cout << "Server shut down gracefully." << std::endl;
	std::cout << "Press Enter to exit." << std::endl;
	std::cin.get();
	return 0;
}


void resolveAddr(const char* port, addrinfo*& outAddrInfo) {
	addrinfo hints{};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;

	int result = getaddrinfo(NULL, port, &hints, &outAddrInfo);
	if (result != 0) {
		throw WinsockException("getaddrinfor failed", result);
	}
	std::cout << "server address info resolved." << std::endl;
}

[[nodiscard]] SocketWrapper createListenSocket(addrinfo* serveraddr) {
	SocketWrapper listenSocket(socket(serveraddr->ai_family, serveraddr->ai_socktype, serveraddr->ai_protocol));
	if (listenSocket.get() == INVALID_SOCKET) {
		throw WinsockException("Socket creation failed", WSAGetLastError());
	}
	std::cout << "Listen socket created successfully." << std::endl;

	if (bind(listenSocket, serveraddr->ai_addr, (int)serveraddr->ai_addrlen) == SOCKET_ERROR) {
		throw WinsockException("bind failed", WSAGetLastError());
	}
	std::cout << "Socket bound successfully." << std::endl;

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		throw WinsockException("listen failed", WSAGetLastError());
	}
	std::cout << "Server is listening on port " << DEFAULT_PORT << std::endl;

	return listenSocket;
}

void handleClient(SocketWrapper clientSocket) {
	SOCKET socket = clientSocket.get();
	std::cout << "Handling client on socket " << clientSocket << std::endl;
	std::vector<char> recvbuf(BUFFER_LEN);
	int result{};
	do {
		result = recv(clientSocket, recvbuf.data(), static_cast<int>(recvbuf.size()), 0);
		if (result > 0) {
			std::string clientMessage(recvbuf.data(), result);
			std::cout << clientSocket << " said: " << clientMessage << std::endl;

			std::lock_guard<std::mutex> lock(clientSocketsMutex);

			for (SOCKET otherClientSocket : clientSockets) {
				if (otherClientSocket != socket) {
					send(otherClientSocket, clientMessage.c_str(), result, 0);
				}
			}
		}
		else if (result == 0) {
			std::cout << "Connection closing... client " << clientSocket << std::endl;
		}
		else {
			std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
			break;
		}
	} while (result > 0);

	{ 
		std::lock_guard<std::mutex> lock(clientSocketsMutex);
		clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), socket), clientSockets.end());
	}

	std::cout << "Finished handling client " << socket << std::endl;
}
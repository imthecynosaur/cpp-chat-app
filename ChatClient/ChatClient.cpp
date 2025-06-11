#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <vector>

#include "WinsockException.h"
#include "SocketWrapper.h"
#include "WSAWrapper.h"

constexpr const char* SERVER_ADDRESS = "127.0.0.1";
constexpr const char* DEFAULT_PORT = "8080";
constexpr int BUFFER_LEN = 512;

void resolveServerAddr(addrinfo*& serverAddrinfo);
SocketWrapper createClientSocket(addrinfo* serverAddrinfo);
void handleServer(SocketWrapper clientSocket);

struct AddrInfoDeleter {
	void operator() (addrinfo* ptr) const {
		std::cout << "AddrInfoDeleter called to free memory." << std::endl;
		if (ptr) {
			freeaddrinfo(ptr);
		}
	}
};

int main() {
	try {
		WSAWrapper wsaData;

		addrinfo* rawAddrPtr{ nullptr };
		resolveServerAddr(rawAddrPtr);
		std::unique_ptr<addrinfo, AddrInfoDeleter> serveraddrinfo(rawAddrPtr);
		SocketWrapper clientSocket = createClientSocket(serveraddrinfo.get());

		std::cout << "trying to send to server" << std::endl;
		handleServer(std::move(clientSocket));
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cout << "Press Enter to exit." << std::endl;
		std::cin.get();
		return 1;
	}
	std::cout << "Press Enter to exit." << std::endl;
	std::cin.get();
	return 0;
}



void resolveServerAddr(addrinfo*& serverAddrinfo) {
	addrinfo hints;
	ZeroMemory(&hints, (int)sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	int result = getaddrinfo(SERVER_ADDRESS, DEFAULT_PORT, &hints, &serverAddrinfo);
	if (result != 0) {
		throw WinsockException("getaddrinfo failed.", result);
	}
	std::cout << "server address resolved successfully." << std::endl;
}

SocketWrapper createClientSocket(addrinfo* serverAddrinfo) {
	for (addrinfo* addrinfoIt{ serverAddrinfo }; addrinfoIt != nullptr; addrinfoIt = addrinfoIt->ai_next) {
		SocketWrapper clientSocket(socket(addrinfoIt->ai_family, addrinfoIt->ai_socktype, addrinfoIt->ai_protocol));
		if (clientSocket == INVALID_SOCKET) {
			std::cout << "Socket creation failed for one address, trying next..." << std::endl;
			continue;
		}
		std::cout << "client socket created successfully. Socket ID: " << clientSocket << std::endl;

		if (connect(clientSocket, addrinfoIt->ai_addr, (int)addrinfoIt->ai_addrlen) == SOCKET_ERROR) {
			std::cout << "Connect failed for one address, trying next..." << std::endl;
			continue;
		}
		std::cout << "Successfully connected to server!" << std::endl;
		return clientSocket;
	}
	throw std::runtime_error("Unable to connect to server after trying all available addresses.");
}

void handleServer(SocketWrapper clientSocket) {
	std::vector<char> recvbuf(BUFFER_LEN);
	do {
		std::cout << "enter your message or type 'quit' to exit" << std::endl;
		std::string userInput;
		std::getline(std::cin, userInput);
		if (userInput == "quit") {
			break;
		}
		if (userInput.empty()) {
			continue;
		}
		int result = send(clientSocket, userInput.c_str(), (int)userInput.length(), 0);
		if (result == SOCKET_ERROR) {
			throw WinsockException("send failed", WSAGetLastError());
		}

		result = recv(clientSocket, recvbuf.data(), static_cast<int>(recvbuf.size()), 0);
		if (result == SOCKET_ERROR) {
			throw WinsockException("recv failed", WSAGetLastError());
		}
		if (result > 0) {
			std::string serverResponse(recvbuf.data(), result);
			std::cout << serverResponse << std::endl;
		}
		else if (result == 0) {
			std::cout << "Connection closed by server." << std::endl;
		}

	} while (true);
}
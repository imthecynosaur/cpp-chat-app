#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <vector>

#define DEFAULT_PORT "8080"

int main() {
	WSADATA wsaData;
	int result;

	struct addrinfo* serverAddrInfo;
	struct addrinfo hints;

	SOCKET serverSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	char recvbuf[512] = { 0 };
	int recvbufLen{ sizeof(recvbuf) };

	std::cout << "starting WSA..." << std::endl;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		std::cerr << "failed to start WSA, error: " << WSAGetLastError() << " And result is: " << result << std::endl;
		return 1;
	}
	std::cout << "WinSock initialized." << std::endl;
	std::cout << "WSAStartup successful." << std::endl;
	std::cout << " Version: " << LOBYTE(wsaData.wVersion) << "." << HIBYTE(wsaData.wVersion) << std::endl;
	std::cout << " Description: " << wsaData.szDescription << std::endl;
	std::cout << " Status: " << wsaData.szSystemStatus << std::endl;
	std::cout << "----------------------" << std::endl;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;

	result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &serverAddrInfo);
	if (result != 0) {
		std::cerr << "failed to resolve server address, error: " << result << std::endl;
		WSACleanup();
		return 1;
	}
	std::cout << "Resolved Server address successfully!" << std::endl;

	if (serverAddrInfo->ai_family == AF_INET) { 
		SOCKADDR_IN* ipv4_addr = reinterpret_cast< SOCKADDR_IN*>(serverAddrInfo->ai_addr);

		char ip_string[INET_ADDRSTRLEN]; 
		inet_ntop(AF_INET, &(ipv4_addr->sin_addr), ip_string, INET_ADDRSTRLEN);
		std::cout << "IP Address resolved by getaddrinfo: " << ip_string << std::endl; 

		
		unsigned short port = ntohs(ipv4_addr->sin_port);
		std::cout << "Port resolved by getaddrinfo: " << port << std::endl; 
		 
	}

	serverSocket = socket(serverAddrInfo->ai_family, serverAddrInfo->ai_socktype, serverAddrInfo->ai_protocol);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(serverAddrInfo);
		WSACleanup();
		return 1;
	}
	std::cout << "Server socket created successfully! Socket number: " << serverSocket << std::endl;

	result = bind(serverSocket, serverAddrInfo->ai_addr, serverAddrInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		std::cerr << "failed to bind socket, error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(serverAddrInfo);
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "Socket bound successfully!" << std::endl;

	freeaddrinfo(serverAddrInfo);

	result = listen(serverSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		std::cerr << "failed to listen, error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "server is now listening on port: " << DEFAULT_PORT << std::endl;
	std::cout << "-----------------------" << std::endl;
	std::cout << "Waiting for incoming connections..." << std::endl;

	clientSocket = accept(serverSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	
	std::cout << "Client connected successfully. Client Socket ID: " << clientSocket << std::endl;
	
	std::cout << "Handling client: " << clientSocket << std::endl;

	std::string clientMessage{};
	std::string serverResponse{};

	do {
		result = recv(clientSocket, recvbuf, recvbufLen, 0);
		if (result < 0) {
			std::cerr << "something fucked up" << WSAGetLastError() << std::endl;
			closesocket(clientSocket);
		}
		else if (result == 0) {
			std::cout << "client disconnected... " << std::endl;
		}
		else {
			std::cout << "receievd " << result << " bytes from client" << std::endl;
			//std::string clientMessage(recvbuf, result);
			clientMessage = std::string(recvbuf, result);
			std::cout << "Client " << clientSocket << " said: " << clientMessage << std::endl;
			serverResponse = "Receieved: " + clientMessage;
			int sendResult = send(clientSocket, serverResponse.c_str(), (int)serverResponse.length(), 0);
			if (sendResult == SOCKET_ERROR) {
				std::cerr << "failed to reply to client, error: " << WSAGetLastError() << std::endl;
				closesocket(clientSocket);
				break;
			}
			std::cout << "sent " << sendResult << " bytes back to client";
		}
		
	} while (result > 0);




	std::cout << "-----------------------------------" << std::endl;

	std::cout << "closing client socket" << std::endl;
	closesocket(clientSocket);
	clientSocket = INVALID_SOCKET;

	std::cout << "Server is shutting down." << std::endl;

	// Close the listening socket
	if (closesocket(serverSocket) == SOCKET_ERROR) {
		std::cerr << "closesocket failed with error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Server socket closed successfully." << std::endl;
	}

	// Cleanup Winsock

	if (WSACleanup() != 0) {
		std::cerr << "WSACleanup failed with error: " << WSAGetLastError() << std::endl;
		return 1;
	}
	else {
		std::cout << "WSACleanup successful." << std::endl;
	}

	std::cout << "Press Enter to exit." << std::endl;
	std::cin.get();

	return 0;
}
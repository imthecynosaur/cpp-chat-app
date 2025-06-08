//#define WIN32_LEAN_AND_MEAN
//
//#include <Windows.h>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <iostream>
//
//#define DEFAULT_PORT "8080"
//
//int main() {
//	WSADATA wsaData;
//	int result;
//
//	SOCKET ListenSocket = INVALID_SOCKET;
//	SOCKET ClientSocket = INVALID_SOCKET;
//	struct addrinfo* resultAddr = NULL;
//	struct addrinfo hints;
//
//	std::cout << "Chat Application Starting..." << std::endl;
//	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (result != 0) {
//		std::cerr << "WSAStartup failed with error: " << result << std::endl;
//		return 1;
//	}
//
//	std::cout << "WSAStartup successful." << std::endl;
//	std::cout << " Version: " << LOBYTE(wsaData.wVersion) << "." << HIBYTE(wsaData.wVersion) << std::endl;
//	std::cout << " Description: " << wsaData.szDescription << std::endl;
//	std::cout << " Status: " << wsaData.szSystemStatus << std::endl;
//
//
//	ZeroMemory(&hints, sizeof(hints));
//	hints.ai_family = AF_INET; // IPv4
//	hints.ai_socktype = SOCK_STREAM; // Stream socket
//	hints.ai_protocol = IPPROTO_TCP; // TCP protocol
//	hints.ai_flags = AI_PASSIVE; // Use my IP address
//
//	char recvbuf[512] = { 0 };
//	int recvbuflen = sizeof(recvbuf);
//
//	result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultAddr);
//	if (result != 0) {
//		std::cerr << "getaddrinfo failed with error: " << result << std::endl;
//		WSACleanup();
//		return 1;
//	}
//
//	std::cout << "getaddrinfo successful." << std::endl;
//
//	ListenSocket = socket(resultAddr->ai_family, resultAddr->ai_socktype, resultAddr->ai_protocol);
//	if (ListenSocket == INVALID_SOCKET) {
//		std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
//		freeaddrinfo(resultAddr);
//		WSACleanup();
//		return 1;
//	}
//	std::cout << "Socket created successfully. Socket ID: " << ListenSocket << std::endl;
//
//	result = bind(ListenSocket, resultAddr->ai_addr, (int)resultAddr->ai_addrlen);
//	if (result == SOCKET_ERROR) {
//		std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
//		freeaddrinfo(resultAddr);
//		closesocket(ListenSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	std::cout << "Socket bound successfully to port " << DEFAULT_PORT << std::endl;
//
//	freeaddrinfo(resultAddr);
//
//
//	result = listen(ListenSocket, SOMAXCONN);
//	if (result == SOCKET_ERROR) {
//		std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
//		closesocket(ListenSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	std::cout << "Listening on port " << DEFAULT_PORT << "..." << std::endl;
//	std::cout << "-------------------------" << std::endl;
//
//	std::cout << "Waiting for incoming connections..." << std::endl;
//	std::cout << "-------------------------" << std::endl;
//
//	ClientSocket = accept(ListenSocket, NULL, NULL);
//
//	if (ClientSocket == INVALID_SOCKET) {
//		std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
//		closesocket(ListenSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	std::cout << "Client connected successfully. Client Socket ID: " << ClientSocket << std::endl;
//
//	std::cout << "Handling client: " << ClientSocket << std::endl;
//
//	do {
//		result = recv(ClientSocket, recvbuf, recvbuflen, 0);
//		if (result > 0) {
//			std::cout << "Bytes received from client " << ClientSocket << ": " << result << std::endl;
//			std::string clientMessage(recvbuf, result);
//			std::cout << "Message: " << clientMessage << std::endl;
//
//			std::string reply = "Message received: " + clientMessage;
//
//			int sendResult = send(ClientSocket, reply.data(), static_cast<int>(reply.size()), 0);
//			if (sendResult == SOCKET_ERROR) {
//				std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
//				closesocket(ClientSocket);
//				break;
//			}
//			std::cout << "Sent " << sendResult << " bytes back to client " << ClientSocket << std::endl;
//		}
//		else if(result == 0){
//				std::cout << "Connection closed by client " << ClientSocket << std::endl;
//		}
//		else {
//				std::cerr << "recv failed for client " << ClientSocket << " with error: " << WSAGetLastError() << std::endl;
//		}
//	} while (result > 0);
//
//
//	std::cout << "Closing connection for client" << ClientSocket << std::endl;
//	closesocket(ClientSocket);
//	ClientSocket = INVALID_SOCKET;
//
//	std::cout << "-----------------------------------" << std::endl;
//
//
//	std::cout << "Server is shutting down." << std::endl;
//
//	// Close the listening socket
//	if (closesocket(ListenSocket) == SOCKET_ERROR) {
//		std::cerr << "closesocket failed with error: " << WSAGetLastError() << std::endl;
//		WSACleanup();
//		return 1;
//	}
//	else {
//		std::cout << "Listening socket closed successfully." << std::endl;
//	}
//
//	// Cleanup Winsock
//
//	if (WSACleanup() != 0) {
//		std::cerr << "WSACleanup failed with error: " << WSAGetLastError() << std::endl;
//		return 1;
//	}
//	else {
//		std::cout << "WSACleanup successful." << std::endl;
//	}
//
//	std::cout << "Press Enter to exit." << std::endl;
//	std::cin.get();
//
//
//	return 0;
//}
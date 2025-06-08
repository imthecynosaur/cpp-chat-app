//#define WIN32_LEAN_AND_MEAN
//
//#include <Windows.h>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//#include <iostream>
//#include <string>
//#include <vector>
//
//#define SERVER_ADDRESS "127.0.0.1"
//#define DEFAULT_PORT "8080"
//#define BUFFER_LEN 512
//
//int main() {
//	WSADATA wsaData;
//	int result;
//
//	SOCKET ClientSocket = INVALID_SOCKET;
//
//	struct addrinfo* addressresult = nullptr;
//	struct addrinfo* addressIterator= nullptr;
//	struct addrinfo hints;
//
//	char recvbuf[BUFFER_LEN];
//
//	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (result != 0) {
//		std::cerr << "WSAStartup failed with error: " << result << std::endl;
//		return 1;
//	}
//	std::cout << "WinSock initialized." << std::endl;
//
//	ZeroMemory(&hints, sizeof(hints));
//	hints.ai_family = AF_UNSPEC;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
//
//	result = getaddrinfo(SERVER_ADDRESS, DEFAULT_PORT, &hints, &addressresult);
//	if (result != 0) {
//		std::cerr << "getaddrinfo failed with error: " << result << std::endl;
//		WSACleanup();
//		return 1;
//	}
//	std::cout << "Server address info resolved." << std::endl;
//
//	for (addressIterator = addressresult; addressIterator != nullptr; addressIterator = addressIterator->ai_next) {
//		ClientSocket = socket(addressIterator->ai_family, addressIterator->ai_socktype, addressIterator->ai_protocol);
//		if (ClientSocket == INVALID_SOCKET) {
//			std::cerr << "Socket creation faild with error: " << WSAGetLastError() << std::endl;
//			freeaddrinfo(addressresult);
//			WSACleanup();
//			return 1;
//		}
//
//		std::cout << "Attempting to connect to server..." << std::endl;
//		result = connect(ClientSocket, addressIterator->ai_addr, (int)addressIterator->ai_addrlen);
//		if (result == SOCKET_ERROR) {
//			std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
//			closesocket(ClientSocket);      // Close this socket attempt
//			ClientSocket = INVALID_SOCKET; // Mark as invalid
//			continue;
//		}
//		std::cout << "Successfully connected to server!" << std::endl;
//		break; // If connected, no need to try further addresses
//	}
//
//	freeaddrinfo(addressresult);
//
//	if (ClientSocket == INVALID_SOCKET) {
//		std::cerr << "Unable to connect to server after trying all addresses!" << std::endl;
//		WSACleanup();
//		return 1;
//	}
//
//	std::string userInput;
//
//	do {
//		std::cout << "Eter message (or type 'exit' to quit): ";
//		std::getline(std::cin, userInput);
//
//		if (userInput == "exit") {
//			std::cout << "Exiting..." << std::endl;
//			break;
//		}
//
//		if (userInput.empty()) {
//			continue;
//		}
//
//		result = send(ClientSocket, userInput.c_str(), (int)userInput.length(), 0);
//		if (result == SOCKET_ERROR) {
//			std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
//			closesocket(ClientSocket);
//			WSACleanup();
//			return 1;
//		}
//		std::cout << "Bytes Sent: " << result << std::endl;
//
//		result = recv(ClientSocket, recvbuf, BUFFER_LEN, 0);
//		if (result > 0) {
//			recvbuf[result] = '\0';
//			std::cout << "Server response: " << recvbuf << std::endl;
//		}
//		else if (result == 0) {
//			std::cout << "Connection closed by server." << std::endl;
//			break;
//		}
//		else {
//			std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
//			break;
//		}
//	} while (true);
//
//	std::cout << "Closing client socket..." << std::endl;
//	result = closesocket(ClientSocket);
//	if (result == SOCKET_ERROR) {
//		std::cerr << "closesocket failed with error: " << WSAGetLastError() << std::endl;
//		// Continue to WSACleanup
//	}
//
//	WSACleanup();
//	std::cout << "Winsock cleaned up." << std::endl;
//
//	std::cout << "Press Enter to exit client...";
//	// A std::cin.ignore() might be needed here if the last std::getline left a newline
//	// in the buffer, but often std::cin.get() will just consume that.
//	// For robustness with mixed cin/getline, an ignore is good practice.
//	// std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
//	std::cin.get();
//
//	return 0;
//}
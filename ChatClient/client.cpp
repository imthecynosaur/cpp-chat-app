#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>

#define SERVER_ADDRESS "127.0.0.1"
#define DEFAULT_PORT "8080"
#define BUFFER_LEN 512

int main() {

	WSADATA wsaData;
	int result;

	addrinfo* addrinforesult;
	addrinfo* addrinfoIterator;
	addrinfo hints;

	SOCKET clientSocket = INVALID_SOCKET;

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	ZeroMemory(&hints, (int)sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	result = getaddrinfo(SERVER_ADDRESS, DEFAULT_PORT, &hints, &addrinforesult);

	for (addrinfoIterator = addrinforesult; addrinfoIterator != nullptr; addrinfoIterator = addrinfoIterator->ai_next) {
		clientSocket = socket(addrinfoIterator->ai_family, addrinfoIterator->ai_socktype, addrinfoIterator->ai_protocol);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "couldn't creat client socket" << WSAGetLastError() << std::endl;
			freeaddrinfo(addrinforesult);
			WSACleanup();
			return 1;
		}
		std::cout << "socket created successfully!" << std::endl;
		
		std::cout << "trying to connect to server" << std::endl;
		result = connect(clientSocket, addrinfoIterator->ai_addr, (int)addrinfoIterator->ai_addrlen);
		if (result == SOCKET_ERROR) {
			std::cerr << "couldn't connect to server" << WSAGetLastError() << std::endl;
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			continue;
		}
		std::cout << "Connected to Server !" << std::endl;
		break;
	}

	freeaddrinfo(addrinforesult);

	std::string userInput;
	char recvbuf[BUFFER_LEN]{};
	std::cout << "trying to send message to server" << std::endl;
	while (true) {
		std::cout << "type in your message or type 'quit' to exit." << std::endl;
		std::getline(std::cin, userInput);
		if (userInput == "quit") {
			closesocket(clientSocket);
			break;
		}
		if (userInput.empty()) {
			continue;
		}
		result = send(clientSocket, userInput.c_str(), (int)userInput.length(), 0);
		if (result == SOCKET_ERROR) {
			std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
		std::cout << result << " BYTES sent." << std::endl;

		result = recv(clientSocket, recvbuf, BUFFER_LEN, 0);
		if (result > 0) {
			recvbuf[result] = '\0';
			std::cout << result << " Bytes received from server: " << std::endl;
			std::cout << recvbuf << std::endl;
		}
		else if (result == 0) {
			std::cout << "Connection closed by server." << std::endl;
			break;
		}
		else {
			std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
			break;
		}
	}

	std::cout << "closing socket" << std::endl;
	result = closesocket(clientSocket);
	if (result == SOCKET_ERROR) {
		std::cerr << "failed to close socket, error: " << WSAGetLastError() << std::endl;
	}

	WSACleanup();
	std::cout << "Winsock closed." << std::endl;

	std::cout << "Press Enter to exit client...";
	// A std::cin.ignore() might be needed here if the last std::getline left a newline
	// in the buffer, but often std::cin.get() will just consume that.
	// For robustness with mixed cin/getline, an ignore is good practice.
	// std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
	std::cin.get();


	return 0;
}
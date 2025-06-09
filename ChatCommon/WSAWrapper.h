#pragma once

#include <WinSock2.h>
#include <iostream>
#include "WinsockException.h"

class WSAWrapper {
public:
	WSAWrapper() {
		WSAData wsaData;
		std::cout << "initializing Winsock..." << std::endl;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0) {
			throw WinsockException("WSAStartup failed", result);
		}
		std::cout << "winsock initialized successfully." << std::endl;
	}

	~WSAWrapper() {
		std::cout << "cleaning up Winsock..." << std::endl;
		int result = WSACleanup();
		if (result != 0) {
			std::cerr << "WSACleanup failed with error: " << WSAGetLastError() << std::endl;
		}
		std::cout << "winsock cleaned up." << std::endl;
	}

	WSAWrapper(const WSAWrapper&) = delete;
	WSAWrapper& operator=(const WSAWrapper&) = delete;
};
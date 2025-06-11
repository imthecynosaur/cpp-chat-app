#pragma once

#include <WinSock2.h>

class SocketWrapper {
public:
	explicit SocketWrapper(SOCKET socket = INVALID_SOCKET) : socket(socket){
		std::cout << "constructor called for socketwrapper. socket ID: " << socket << std::endl;
	}

	~SocketWrapper() {
		std::cout << "calling Socket Wrapper destructor. Socket ID: " << socket << std::endl;
		if (socket != INVALID_SOCKET) {
			closesocket(socket);
			std::cout << "Socket closed from destructor if clause. Socket ID: " << socket << std::endl;
		}
	}

	SocketWrapper(SocketWrapper&& other) noexcept : socket(other.socket) {
		std::cout << "move constructor called for socketwrapper. socket ID: " << socket << std::endl;
		other.socket = INVALID_SOCKET;
	}

	SocketWrapper& operator=(SocketWrapper&& other) noexcept {
		if (this != &other) {
			std::cout << "move assignment oprator called for socketwrapper. socket ID: " << socket << std::endl;
			if (socket != INVALID_SOCKET) {
				closesocket(socket);
			}
			socket = other.socket;
			other.socket = INVALID_SOCKET;
		}
		return *this;
	}

	SocketWrapper(const SocketWrapper&) = delete;
	SocketWrapper& operator=(const SocketWrapper&) = delete;

	operator SOCKET() const{
		return socket;
	}

	SOCKET get() const {
		return socket;
	}

private:
	SOCKET socket{INVALID_SOCKET};
};
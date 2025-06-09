#pragma once

#include <WinSock2.h>

class SocketWrapper {
public:
	explicit SocketWrapper(SOCKET socket = INVALID_SOCKET) : socket(socket){}

	~SocketWrapper() {
		if (socket != INVALID_SOCKET) {
			closesocket(socket);
		}
	}

	SocketWrapper(SocketWrapper&& other) noexcept : socket(other.socket) {
		other.socket = INVALID_SOCKET;
	}

	SocketWrapper& operator=(SocketWrapper&& other) noexcept {
		if (this != &other) {
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
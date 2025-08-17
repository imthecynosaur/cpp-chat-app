#define WIN32_LEAN_AND_MEAN

#include "Server.h"
#include "WSAWrapper.h"

#include <iostream>

constexpr const char* DEFAULT_PORT = "8080";

int main() {
	try {
		WSAWrapper wsa;
		Server server(DEFAULT_PORT);
		server.run();
	}
	catch(const std::exception& e){
		std::cerr << "Error: " << e.what() << std::endl;
		std::cout << "Press Enter to exit." << std::endl;
		std::cin.get();
		return 1;
	}
	return 0;
}
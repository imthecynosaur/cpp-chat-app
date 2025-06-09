#pragma once

#include <stdexcept>
#include <string>

class WinsockException : public std::runtime_error {
public:
	WinsockException(const std::string& message, int errorCode) :
		std::runtime_error(message + " - Error code: " + std::to_string(errorCode)),
		errorCode(errorCode){}

	int getErrorCode() const noexcept {
		return errorCode;
	}

private:
	int errorCode{};
};
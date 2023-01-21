#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdexcept>
#include <utility>
#include "Network/ClientSocket.h"

using namespace network;
using namespace std;

ClientSocket::~ClientSocket() {
	if (_socketID != -1) {
		close(_socketID);
	}
}

ClientSocket::ClientSocket(ClientSocket&& other) {
	*this = std::move(other);
}

ClientSocket& ClientSocket::operator=(ClientSocket&& other) {
	_socketID = other._socketID;
	other._socketID = -1;
	
	return *this;
}

void ClientSocket::Send(const string& dataToSend) const {
	string data = dataToSend + "\0";
	if (send(_socketID, data.c_str(), data.size(), 0) == -1) {
		throw runtime_error(std::string("Error sending with client socket! Error code: ") + std::to_string(errno));
	}
}

string ClientSocket::Receive(int maxBufferSize) const {
	char buf[4096];
	memset(buf, 0, 4096);
	int bytesReceived = recv(_socketID, buf, sizeof(buf), 0);
	
	if (bytesReceived == -1) {
		throw runtime_error(std::string("Error receiving on client socket! Error code: ") + std::to_string(errno));
	}
	
	return string(buf, bytesReceived);
}

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdexcept>
#include <utility>
#include "Network/ClientSocket.h"

using namespace network;
using namespace std;


ClientSocket::ClientSocket(int socketID) : 
	socketID(socketID) {
    struct sockaddr_in localAddress;
    socklen_t addrSize = sizeof(localAddress);
    getsockname(socketID, reinterpret_cast<sockaddr*>(&localAddress), &addrSize);
    port = localAddress.sin_port;
    ip = localAddress.sin_addr.s_addr;
}

ClientSocket::~ClientSocket() {
	if (socketID != -1) {
		close(socketID);
	}
}

ClientSocket::ClientSocket(ClientSocket&& other) {
	*this = std::move(other);
}

ClientSocket& ClientSocket::operator=(ClientSocket&& other) {
	socketID = other.socketID;
	port = other.port;
	ip = other.ip;
	other.socketID = -1;
	
	return *this;
}

void ClientSocket::send(const string& dataToSend) const {
	string data = dataToSend + "\0";
	if (::send(socketID, data.c_str(), data.size(), 0) == -1) {
		throw runtime_error(std::string("Error sending with client socket! Error code: ") + std::to_string(errno));
	}
}

string ClientSocket::receive(int maxBufferSize) const {
	char buf[MAX_PACKET_SIZE] = {0};
	int bytesReceived = recv(socketID, buf, maxBufferSize, 0);
	
	if (bytesReceived == -1) {
		throw runtime_error(std::string("Error receiving on client socket! Error code: ") + std::to_string(errno));
	}

	if (bytesReceived == 0) {
		throw runtime_error(std::string("Client disconnected"));
	}
	
	return string(buf, bytesReceived);
}

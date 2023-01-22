#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdexcept>
#include "Network/Server.h"

using namespace std;
using namespace network;

Server::Server(int port) {
	// Create a socket
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1) {
		throw runtime_error(std::string("Couldn't create server socket! Error code: ") + std::to_string(errno));
    }
 
    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);
	
	if (bind(_serverSocket, (sockaddr*)&hint, sizeof(hint)) == -1) {
		throw runtime_error(std::string("Couldn't bind server socket! Error code: ") + std::to_string(errno));
	}
 
    // Tell Winsock the socket is for listening
    if (listen(_serverSocket, SOMAXCONN) == -1) {
		throw runtime_error(std::string("Couldn't listen to server socket! Error code: ") + std::to_string(errno));
	}
}

Server& Server::operator=(Server&& other) {
	_serverSocket = other._serverSocket;
	other._serverSocket = -1;
	
	return *this;
}

Server::~Server() {
	if (_serverSocket != -1) {
		close(_serverSocket);
	}
}

ClientSocket Server::AcceptNewClient() {	
	int clientSocketID = accept(_serverSocket, NULL, NULL);
	
    if (clientSocketID == -1) {
		throw std::runtime_error(std::string("Couldn't accept a client socket! Error code: ") + std::to_string(errno));
    }
 
    return ClientSocket(clientSocketID);
}

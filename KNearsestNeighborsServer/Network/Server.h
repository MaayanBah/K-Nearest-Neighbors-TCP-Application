#ifndef __CLIENT_SERVER_H
#define __CLIENT_SERVER_H

#include "Network/ClientSocket.h"

namespace network
{
	class Server
	{
	private:
		// Data Members
		int _serverSocket;
		
	public:
		// C'tors
		Server() : _serverSocket(-1) { }
		Server(int port);
		Server(const Server&) = delete;
		Server(Server&&) = delete;
		
		// D'tor
		~Server();
		
		// Assignment Operators
		Server& operator=(const Server&) = delete;
		Server& operator=(Server&&);
		
		// Methods
		ClientSocket AcceptNewClient();
	};
}

#endif

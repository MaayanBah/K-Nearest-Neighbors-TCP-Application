#ifndef __CLIENT_SOCKET_H
#define __CLIENT_SOCKET_H

#include <string>

namespace network
{
	class ClientSocket
	{
	private:
		// Data Members
		int _socketID;
		
	public:
		// C'tors
		ClientSocket() : 
			_socketID(-1) { }
		ClientSocket(int socketID) : 
			_socketID(socketID) { }
		ClientSocket(const ClientSocket&) = delete;
		ClientSocket(ClientSocket&&);
		
		// D'tor
		~ClientSocket();
		
		// Assignment Operators
		ClientSocket& operator=(const ClientSocket&) = delete;
		ClientSocket& operator=(ClientSocket&& other);
		
		// Methods
		void send(const std::string& dataToSend) const;
		std::string receive(int maxBufferSize = 4096) const;
	};
}

#endif

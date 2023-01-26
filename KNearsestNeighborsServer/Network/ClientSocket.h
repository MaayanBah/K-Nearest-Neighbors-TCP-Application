#ifndef __CLIENT_SOCKET_H
#define __CLIENT_SOCKET_H

#include <string>

namespace network
{
	class ClientSocket
	{
	private:
		// Consts
		static const unsigned int MAX_PACKET_SIZE = 16384;

		// Data Members
		int socketID;
		unsigned int ip;
		unsigned short port;
		
	public:
		// C'tors
		ClientSocket() : 
			socketID(-1),
			ip(0),
			port(0) { }
		ClientSocket(int socketID);
		ClientSocket(const ClientSocket&) = delete;
		ClientSocket(ClientSocket&&);
		
		// D'tor
		~ClientSocket();
		
		// Assignment Operators
		ClientSocket& operator=(const ClientSocket&) = delete;
		ClientSocket& operator=(ClientSocket&& other);
		
		// Methods
		inline unsigned int getIP() const { return ip; }
		inline unsigned short getPort() const { return port; }
		void send(const std::string& dataToSend) const;
		std::string receive(int maxBufferSize = MAX_PACKET_SIZE) const;
	};
}

#endif

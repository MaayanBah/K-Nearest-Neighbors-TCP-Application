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
		void Send(const std::string& dataToSend) const;
		std::string Receive(int maxBufferSize = 4096) const;
	};
}

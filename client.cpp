#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unordered_set>

using namespace std;

bool validateClientInput(const string& clientData)
{
	stringstream stream(clientData);
	
    // Parse the vector
    do {
        double currentNumber;
        stream >> currentNumber;
    } while (!stream.eof() && !stream.fail());
	
	if (stream.eof()) {
		return false;
	}
	
	// Parse the distance
	std::string distanceString;
	stream.clear();
	stream >> distanceString;
	
	if (stream.fail()) {
		return false;
	}	
	
	const unordered_set<string> conversionMap = {"AUC", "MAN", "CHB", "CAN", "MIN"};

    if (conversionMap.find(distanceString) == conversionMap.cend()) { 
        return false;
    }
	
	int k;
	// Parse k
    stream >> k;
	
	return !stream.fail();
}

int main(int argc, char* argv[]) {
	argc--;
	if (argc != 2)
	{
		cout << "Invalid argument count: " << argc << endl;
		return 0;
	}
	
    string ipAddress = argv[1];
	
	int port;
	stringstream stream(argv[2]);
	stream >> port;
		
	if (stream.fail())
	{
		cout << "Invalid port given: " << argv[2] << endl;
		return 0;
	}
	
	if (port > 65535 || port < 0)
	{
		cout << "Port given is not a legal port: " << port << endl;
		return 0;
	}
	
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
		cout << "Failed to create socket" << endl;
        return 0;
    }
	
    // Create a hint structure for the server we are connecting with
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
	
    if (inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr) == 0)
	{
		cout << ipAddress << " is not a valid ip address" << endl;
		return 0;
	}

    // Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1) {
		cout << "Failed to connect to socket with ip " << ipAddress << " and port " << port << endl;
        return 0;
    }

    // while loop:
    char buf[4096];
    string userInput;

    do {
        // Enter lines of text 
        getline(cin, userInput);
		
		while (userInput != "-1" && !validateClientInput(userInput)) {
			cout << "invalid input\n";
			getline(cin, userInput);
		}
		
		if (userInput == "-1") {
			break;
		}

        // Send to server
        if (send(sock, userInput.c_str(), userInput.size() + 1, 0) == -1) {
            cout << "Couldn't send to server! Whoops!" << endl;
            continue;
        }

        // Wait for a response
        memset(buf, 0, sizeof(buf));
        int bytesReceived = recv(sock, buf, sizeof(buf), 0);
        if (bytesReceived == -1) {
            cout << "There was an error getting response from server" << endl;
			break;
        }
		
		if (bytesReceived == 0) {
			cout << "Server unavailable, the socket disconnected.\n";
			break;
		}
		
		cout << string(buf, bytesReceived) << endl;
    } while(true);

    close(sock);
    return 0;
}
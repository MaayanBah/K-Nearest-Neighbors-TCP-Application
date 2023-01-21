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

//check if number or string
bool checkNumber(const string& str) {
   for (int i = 0; i < str.length(); i++) {
		if (isdigit(str[i]) == false) {
			return false;
		}
   }
   return true;
}

void responseFromServer(const int& sock) {
	char buf[4096];
	// Wait for a response from the server
	memset(buf, 0, sizeof(buf));
	int bytesReceived = recv(sock, buf, sizeof(buf), 0);
	if (bytesReceived == -1) {
		cout << "There was an error getting response from server" << endl;
		return;
	}
	
	if (bytesReceived == 0) {
		cout << "Server unavailable, the socket disconnected.\n";
		return;
	}
	
	cout << string(buf, bytesReceived) << endl;
}

void sendToServer(const int& sock, const string& inputFromUserToSend) {
	if (send(sock, inputFromUserToSend.c_str(), inputFromUserToSend.size() + 1, 0) == -1) {
		cout << "Couldn't send to server! Whoops!" << endl;
		return;
	}
}

void uploadUnclassifiedCSV(const int& sock) {
	string localTrainFile;
	cout << "Please upload your local train CSV file." << endl;
	getline(cin, localTrainFile);
	//cin >> localTrainFile;
	// Send to server
	sendToServer(sock, localTrainFile);
	responseFromServer(sock);
	string localTestFile;
	cout << "Please upload your local test CSV file." << endl;
	//cin >> localTestFile;
	getline(cin, localTestFile);
	// Send to server
	sendToServer(sock, localTestFile);
	responseFromServer(sock);
}

void algorithemSettings(const int& sock) {
	// here we should get a response from the server of the
	// initial k and metric (Maayan's implement)
	//responseFromServer(sock); ----> to uncomment it when Maayan finish implement
	string userInput;
	// get k and metric from user
	cin.ignore();
	getline(cin, userInput);
	if (userInput.length() == 0) {
		//cout << "it was an Enter" << endl;
		// Maayan implement - should reponse the menu
		//responseFromServer(sock);
	} else {
		// send them to the server
		sendToServer(sock, userInput);
		// Maayan implement - if its valid - response back the menu
		// else 'invalid value for metric' or/and 'invalid value for K'
		responseFromServer(sock);
	}
}

void classifyData(const int& sock) {
	// waiting for a response from server
	// Maayan implement : the server runs the CSV files from option 1
	// and server response back 'classifying data complete' or 'please upload data'
	// the get back to the menu
	responseFromServer(sock);
}

void displayResults(const int& sock) {
	string userInput;
	// the server response list of classifields, or 'plase upload data' / 'please classify the data'
	responseFromServer(sock);
	// after we get the response, if the user use the enter key, we get back to the menu
	cin.ignore();
	getline(cin, userInput);
	if (userInput.length() == 0) {
		//cout << "it was an Enter" << endl;
		// Maayan implement - should reponse the menu
		responseFromServer(sock);
	}
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


    string userInput;
	bool shouldExit = false;
	
    do {
		responseFromServer(sock);
		
		// after the menu, the user enters a number according to the menu options
        getline(cin, userInput);

		// If the client didn't enter a number
		if (checkNumber(userInput) == false) {
			cout << "You should enter a number from the menu" << endl;
			continue;
		}
		
		sendToServer(sock, userInput);

		switch (stoi(userInput)) {
			case 1:
				uploadUnclassifiedCSV(sock);
				// Maayan need to implement: "Upload complete.""
				// or "invalid input"
				// as a feedback of the server
				break;
			case 2:
				algorithemSettings(sock);
				break;
			case 3:
				classifyData(sock);
				break;
			case 4:
				displayResults(sock);
				break;
			case 5:
				// will implement later
				break;
			case 8:
				shouldExit = true;
				break;
			default:
				cout << "An invalid option" << endl;
			}
    } while(!shouldExit);

    close(sock);
    return 0;
}
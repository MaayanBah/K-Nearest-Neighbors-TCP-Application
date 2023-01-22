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
#include <fstream>

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

void printResponseFromServer(const int& sock) {
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

string returnResponseFromServer(const int& sock) {
	char buf[4096];
	// Wait for a response from the server
	memset(buf, 0, sizeof(buf));
	int bytesReceived = recv(sock, buf, sizeof(buf), 0);
	if (bytesReceived == -1) {
		cout << "There was an error getting response from server" << endl;
		return "Error";
	}
	
	if (bytesReceived == 0) {
		cout << "Server unavailable, the socket disconnected.\n";
		return "Error";
	}
	return string(buf, bytesReceived);
}

void sendToServer(const int& sock, const string& inputFromUserToSend) {
	if (send(sock, inputFromUserToSend.c_str(), inputFromUserToSend.size() + 1, 0) == -1) {
		cout << "Couldn't send to server! Whoops!" << endl;
		return;
	}
}

void sendToServer(const int& sock, const string& inputFromUserToSend, const int& bytesToSend) {
	if (send(sock, inputFromUserToSend.c_str(), bytesToSend + 1, 0) == -1) {
		cout << "Couldn't send to server! Whoops!" << endl;
		return;
	}
}

void sendFileToServerByChuncks(const int& sock, string localTrainFile) {
	const int FILE_CHUNK_SIZE = 4095; 

	// Check if file exists
	ifstream file(localTrainFile, ios::binary);
	if(!file) {
		cout<<"invalid input" << endl;
		return;
	}

	// Get file size
	file.seekg(0, ios::end);
	unsigned int fileSize = file.tellg();
	file.close();

	// Get the file
	char* fileBuffer = new char[fileSize];
	file.open(localTrainFile, ios::binary);
	file.seekg (0, ios::beg);
	file.read (fileBuffer, fileSize);
	file.close();

	// Send file in chunks
	unsigned int bytesSent = 0;
	int bytesToSend = 0;
	while(bytesSent < fileSize) {
		if(fileSize - bytesSent >= FILE_CHUNK_SIZE) {
			bytesToSend = FILE_CHUNK_SIZE;
		} else {
			bytesToSend = fileSize - bytesSent;
		}
		string initial = "0";
		string buf = initial.append(fileBuffer + bytesSent);
		// Send the file in chunks (with initial "0") to server and wait for a response of the server
		sendToServer(sock, buf, bytesToSend+1);
		//cout << buf;
		//bytesSent += bytesToSend;

		// if the server confirm to get the chunk
		if (returnResponseFromServer(sock) == "1") {
			bytesSent += bytesToSend;
		} else if (returnResponseFromServer(sock) == "2") {
			cout << "There was an error with the content of the file" << endl;
			return;
		}
	}
	delete [] fileBuffer;
	
	// Let the server know we finished the sending
	sendToServer(sock, "1");
	// should be "upload complete."
	printResponseFromServer(sock);
}

void uploadUnclassifiedCSV(const int& sock) {
	string localTrainFile;
	cout << "Please upload your local train CSV file." << endl;
	getline(cin, localTrainFile); 
	sendFileToServerByChuncks(sock, localTrainFile);

	string localTestFile;
	cout << "Please upload your local test CSV file." << endl;
	getline(cin, localTestFile);
	sendFileToServerByChuncks(sock, localTestFile);
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
		printResponseFromServer(sock);
	}
}

void classifyData(const int& sock) {
	// waiting for a response from server
	// Maayan implement : the server runs the CSV files from option 1
	// and server response back 'classifying data complete' or 'please upload data'
	// the get back to the menu
	printResponseFromServer(sock);
}

void displayResults(const int& sock) {
	string userInput;
	// the server response list of classifields, or 'plase upload data' / 'please classify the data'
	printResponseFromServer(sock);
	// after we get the response, if the user use the enter key, we get back to the menu
	cin.ignore();
	getline(cin, userInput);
	if (userInput.length() == 0) {
		//cout << "it was an Enter" << endl;
		// Maayan implement - should reponse the menu
		printResponseFromServer(sock);
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
		// uncomment it when Maayan implement the menu
		//responseFromServer(sock);
		cout << "a manu" << endl;
		
		// after the menu, the user enters a number according to the menu options
        getline(cin, userInput);

		// If the client didn't enter a number
		if (checkNumber(userInput) == false) {
			cout << "An invalid option" << endl;
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
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
#include <thread>
#include <vector>
#include <list>
#include <stdexcept>

using namespace std;

int createSocket(const string& ipAddress, const int& port) {
 	// Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
		throw std::runtime_error("Failed to create socket");
    }
	
    // Create a hint structure for the server we are connecting with
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
	
    if (inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr) == 0)
	{
		throw std::runtime_error(ipAddress + " is not a valid ip address");
	}

    // Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1) {
		throw std::runtime_error("Failed to connect to socket with ip " + ipAddress + " and port " + to_string(port));
    }

	return sock;
}

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

void sendToServer(const int& sock, const string& inputFromUserToSend) {
	if (send(sock, inputFromUserToSend.c_str(), inputFromUserToSend.size(), 0) == -1) {
		throw std::runtime_error("Couldn't send to server!");
	}
}

string returnResponseFromServer(const int& sock) {
	char buf[4096];
	// Wait for a response from the server
	memset(buf, 0, sizeof(buf));
	int bytesReceived = recv(sock, buf, sizeof(buf), 0);
	if (bytesReceived == -1) {
		throw std::runtime_error("There was an error getting response from server");
	}
	
	if (bytesReceived == 0) {
		throw std::runtime_error("Server unavailable, the socket disconnected");
	}
	
	// The server expects a response for every time it sends data.
	// The server behaves this way so that two seperate sends 
	// from the server's side wouldn't accidentally get chained together
	sendToServer(sock, "1");
	return string(buf, bytesReceived);
}

bool sendFileToServerByChunks(const int& sock, string localTrainFile) {
	const int FILE_CHUNK_SIZE = 16383; 

	// Check if file exists
	ifstream file(localTrainFile, ios::binary);
	if(!file) {
		cout << "invalid input" << endl;
		sendToServer(sock, "2");
		returnResponseFromServer(sock);
		return false;
	}

	// Get file size
	file.seekg(0, ios::end);
	unsigned int fileSize = file.tellg();
	file.close();

	// Get the file
	vector<char> fileBuffer(fileSize);
	file.open(localTrainFile, ios::binary);
	file.seekg (0, ios::beg);
	file.read (fileBuffer.data(), fileSize);
	file.close();

	// Send file in chunks
	unsigned int bytesSent = 0;
	int bytesToSend = 0;
	string response;
	
	while(bytesSent < fileSize) {
		if(fileSize - bytesSent >= FILE_CHUNK_SIZE) {
			bytesToSend = FILE_CHUNK_SIZE;
		} else {
			bytesToSend = fileSize - bytesSent;
		}
		string initial = "0";
		string buf = initial.append(string(fileBuffer.data() + bytesSent, fileBuffer.data() + bytesSent + bytesToSend));
		// Send the file in chunks (with initial "0") to server and wait for a response of the server
		sendToServer(sock, buf);
		
		response = returnResponseFromServer(sock);
		
		// if the server confirm to get the chunk
		if (response[0] == '1') {
			bytesSent += bytesToSend;
		} else if (response[0] == '2') {
			cout << "invalid input" << endl;
			return false;
		}
		else
		{
			cout << "Server responded with invalid header" << endl;
			return false;
		}
	}
	
	// Let the server know we finished the sending
	sendToServer(sock, "1");
	
	response = returnResponseFromServer(sock);
	if (response[0] == '2') {
		cout << "invalid input" << endl;
		return false;
	}
	
	// should be "upload complete."
	cout << response;
	
	return true;
}

void uploadUnclassifiedCSV(const int& sock) {
	string localTrainFile;
	cout << "Please upload your local train CSV file." << endl;
	getline(cin, localTrainFile); 
	if (!sendFileToServerByChunks(sock, localTrainFile)) {
		return;
	}

	string localTestFile;
	cout << "Please upload your local test CSV file." << endl;
	getline(cin, localTestFile);
	
	sendFileToServerByChunks(sock, localTestFile);
}


void algorithemSettings(const int& sock) {
	cout << returnResponseFromServer(sock);
	string userInput;
	getline(cin, userInput);
	
	if (userInput.empty()) {
		userInput = string(1, '\0');
	}
	
	sendToServer(sock, userInput);
	string response = returnResponseFromServer(sock);
	if (response[0] != '1') {
		cout << response;
	}
}

void classifyData(const int& sock) {
	cout << returnResponseFromServer(sock);
}

void displayResults(const int& sock) {
	do {
		string buf = returnResponseFromServer(sock);
		if (!buf.empty() && buf[0] == '0') {
			sendToServer(sock, "1");
			cout << buf.substr(1);
		} else {
			if (!buf.empty() && buf[0] == '1') {
				sendToServer(sock, "1");
				cout << "Done.\n";
			} else if (buf[0] == '2') {
				cout << "There was a problem receiving the content" << endl;					
				sendToServer(sock, "2");
			} else {
				cout << buf;
			}
			
			break;
		}
	} while(true);
}

void getTheContentToFile(const int& sock, const string& filePath) {
	std::ofstream outfile (filePath);
	
	do {
		string buf = returnResponseFromServer(sock);
		if (!buf.empty() && buf[0] == '0') {
			sendToServer(sock, "1");
			outfile << buf.substr(1);
		} else {
			if (!buf.empty() && buf[0] == '1') {
				sendToServer(sock, "1");
			} else if (buf[0] == '2') {
				cout << "There was a problem receiving the content" << endl;					
				sendToServer(sock, "2");
			} else {
				cout << buf;
			}
			
			break;
		}
	} while(true);
}

void downloadResults(const string& filePath, const string& ipAddress, int port, unsigned int sockFatherPort) {
	// everytime the user enters a path to a file (find or create if doesn't exists)
	// we open a thread, so this thread actually uses a function, so that function
	// actually saves the content into this file (in chunks like operation 4).
	// in the meantime, the father thread doesn't wait to its child
	// and prints (as a receive from server) the menu again
	// if the user enters 5 again, and a path to a file, we open one more thread
	// so we have parallel threads, working both together
	
	// create a new socket with the server
	// and dont print its menu default response
	int sock;
	try {
		sock = createSocket(ipAddress, port);
	} catch (std::runtime_error e) {
		cout << e.what() << endl;
		return;
	}
	
	try {
		// getting menu from the server, and doesn't print it
		returnResponseFromServer(sock);
		
		// send a special command to server to open a communication
		sendToServer(sock, "0");
		
		if (returnResponseFromServer(sock)[0] != '1') {
			cout << "There was a problem receiving the content" << endl;
		} else {
			sendToServer(sock, to_string(sockFatherPort));
			string response = returnResponseFromServer(sock);
			
			if (response[0] != '1') {
				cout << "There was a problem receiving the content" << endl;
			} else {
				returnResponseFromServer(sock);
				
				sendToServer(sock, "4");
				
				getTheContentToFile(sock, filePath);	
			}
		}
	} catch (std::runtime_error e) {
		cout << e.what() << endl;
	}
	
    close(sock);
}

void sendSockPortAndDownload(const int& sock, 
							 const string& serverIP,
							 int serverPort,
							 std::list<std::thread>& vecOfThreads) {	
    sockaddr_in hint;
	socklen_t hint_length = sizeof(hint);
	
	getsockname(sock, (struct sockaddr *) &hint, &hint_length);
	string response = returnResponseFromServer(sock);
	
	if (response[0] != '1') {
		cout << response;
		return;
	}
	
	sendToServer(sock, to_string(hint.sin_port));
	
	response = returnResponseFromServer(sock);
	if (response[0] != '1') {
		cout << "Failed to download file from server" << endl;
		return;
	}
	
	string pathToFile;
	getline(cin, pathToFile);
	
	sendToServer(sock, "1");
	
	vecOfThreads.push_back(thread(downloadResults, 
								  pathToFile, 
								  serverIP, 
								  serverPort, 
								  hint.sin_port));
}

bool parseInteger(const string& integerString, int& parsedInteger) {
    stringstream stream(integerString);
    stream >> parsedInteger;

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
	int sock;
	try {
		sock = createSocket(ipAddress, port);
	} catch (std::runtime_error e) {
		cout << e.what() << endl;
	}
	
	try {		
		string userInput;
		std::list<std::thread> vecOfThreads;
		bool shouldExit = false;

		do {
			cout << returnResponseFromServer(sock);
			
			// after the menu, the user enters a number according to the menu options
			getline(cin, userInput);
			
			int choice;

			// If the client didn't enter a number
			while (!parseInteger(userInput, choice) || (choice > 5 && choice != 8) || choice < 1) {
				cout << "invalid input" << endl;
				getline(cin, userInput);
			}
			
			// send the server the user choice
			sendToServer(sock, userInput);

			switch (choice) {
				case 1:
					uploadUnclassifiedCSV(sock);
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
					sendSockPortAndDownload(sock, ipAddress, port, vecOfThreads);
					break;
				case 8:
					shouldExit = true;
					break;
			}
		} while(!shouldExit);
	} catch (std::runtime_error e) {
		cout << e.what() << endl;
	}

    close(sock);
    return 0;
}
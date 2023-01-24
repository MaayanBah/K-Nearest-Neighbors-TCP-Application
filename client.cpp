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
using namespace std;

int createSocket(const string& ipAddress, const int& port) {
 	// Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
		cout << "Failed to create socket" << endl;
        exit;
    }
	
    // Create a hint structure for the server we are connecting with
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
	
    if (inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr) == 0)
	{
		cout << ipAddress << " is not a valid ip address" << endl;
		exit;
	}

    // Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1) {
		cout << "Failed to connect to socket with ip " << ipAddress << " and port " << port << endl;
        exit;
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
		exit;
	}
	
	if (bytesReceived == 0) {
		cout << "Server unavailable, the socket disconnected.\n";
		exit;
	}
	return string(buf, bytesReceived);
}

void sendToServer(const int& sock, const string& inputFromUserToSend) {
	if (send(sock, inputFromUserToSend.c_str(), inputFromUserToSend.size() + 1, 0) == -1) {
		cout << "Couldn't send to server! Whoops!" << endl;
		exit;
	}
}

void sendToServer(const int& sock, const string& inputFromUserToSend, const int& bytesToSend) {
	if (send(sock, inputFromUserToSend.c_str(), bytesToSend + 1, 0) == -1) {
		cout << "Couldn't send to server! Whoops!" << endl;
		exit;
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
	// print the initial k and metric (Maayan's implement) - initialized to 5 and EUC
	printResponseFromServer(sock); 	// to uncomment it when Maayan finish implement
	string userInput;
	// get k and metric from user
	getline(cin, userInput);
	sendToServer(sock, userInput);
	// Maayan implement - if its valid - response back the menu
	// else 'invalid value for metric' or/and 'invalid value for K'
	printResponseFromServer(sock);
}

void classifyData(const int& sock) {
	// waiting for a response from server
	// Maayan implement : the server runs the CSV files from option 1
	// and server response back 'classifying data complete' or 'please upload data'
	// then get back to the menu
	printResponseFromServer(sock);
}

void displayResults(const int& sock) {
	// get the contect from the server (till 4096 bytes and including initial "0" for each chunk)
	do {
		string buf = returnResponseFromServer(sock);
		// if content is alright - then delete the initial "0" of chunk and print it
		if (buf.length() > 0 && buf[0] == '0') {
			sendToServer(sock, "1");
			buf.erase(buf.begin());
			cout << buf;
		}
		// if content is invalid - then send an error and print a message
		if (buf.length() <= 0 || (buf[0] != '0' && buf != "1")) {
			sendToServer(sock, "2");
			cout << "There was a problem receiving the contect" << endl;
			exit;
		}
		// if there is no more content to read, server send 1, and client exits
		if (buf == "1") {
			break;
		}
	} while(true);
}

void getTheContectToFile(const int& sock, const string& filePath) {
	// get the contect from the server (till 4096 bytes and including initial "0" for each chunk)
	do {
		string buf = returnResponseFromServer(sock);
		// if content is alright - then delete the initial "0" of chunk and print it
		if (buf.length() > 0 && buf[0] == '0') {
			sendToServer(sock, "1");
			buf.erase(buf.begin());
			std::ofstream outfile (filePath);
			outfile << buf << std::endl;
		}
		// if content is invalid - then send an error and print a message
		if (buf.length() <= 0 || (buf[0] != '0' && buf != "1")) {
			sendToServer(sock, "2");
			cout << "There was a problem receiving the contect" << endl;
			exit;
		}
		// if there is no more content to read, server send 1, and client exits
		if (buf == "1") {
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
	
	//Print Thread ID and path to file
	std::cout << "From Thread ID : "<<std::this_thread::get_id() << "\n"; // to commit it 
	cout << "the path is: " << filePath << endl; // to commit it

	// create a new socket with the server
	// and dont print its menu default response
	int sock = createSocket(ipAddress, port);
	cout << "sock number inside function: " << sock << endl; // to commit
	// getting menu from the server, and doesn't print it
	string menu = returnResponseFromServer(sock);
	// send a special command to server to open a communication
	sendToServer(sock, "6");
	if (returnResponseFromServer(sock) == "1") {
		sendToServer(sock, to_string(sockFatherPort));
		getTheContectToFile(sock, filePath);

	}

	


}


bool checkIfFileExists(const string& pathToFile) {
	ifstream file(pathToFile, ios::binary);
	if(!file) {
		return false;
	} 
	return true;
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
	
	// create a socket and connection to server
	//int sock = createSocket(ipAddress, port);

 	// Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
		cout << "Failed to create socket" << endl;
        exit;
    }
	
    // Create a hint structure for the server we are connecting with
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
	socklen_t hint_length = sizeof(hint);

	
    if (inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr) == 0)
	{
		cout << ipAddress << " is not a valid ip address" << endl;
		exit;
	}

    // Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1) {
		cout << "Failed to connect to socket with ip " << ipAddress << " and port " << port << endl;
        exit;
    }

	getsockname(sock, (struct sockaddr *) &hint, &hint_length);
    std::cout << "Port number of main socket: " << ntohs(hint.sin_port) << std::endl;

    string userInput;
	std::vector<std::thread> vecOfThreads;
	bool shouldExit = false;
	
    do {
		// uncomment it when Maayan implement the menu
		//responseFromServer(sock);
		cout << "a manu" << endl; // commit it
		
		// after the menu, the user enters a number according to the menu options
        getline(cin, userInput);

		// If the client didn't enter a number
		if (checkNumber(userInput) == false) {
			cout << "An invalid option" << endl;
			continue;
		}
		
		// send the server the user choice
		sendToServer(sock, userInput);
		string pathToFile;

		switch (stoi(userInput)) {
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
				cout << "sock number in switch case: " << sock << endl; // to commit
				cout << "Please upload a path to file" << endl; //to commit it
				getline(cin, pathToFile);
				// Check if file exists
				if (checkIfFileExists(pathToFile)) {
					// file exists
					cout << "file exists!" << endl; // commit it
					vecOfThreads.push_back(thread(downloadResults, pathToFile, ipAddress, port, ntohs(hint.sin_port)));
				} else {
					// file doesn't exists - then create it
					std::ofstream outfile (pathToFile);
					//outfile << "my text here!" << std::endl; // to commit it
					vecOfThreads.push_back(thread(downloadResults, pathToFile, ipAddress, port, ntohs(hint.sin_port)));
				}
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
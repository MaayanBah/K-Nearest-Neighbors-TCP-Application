#include <iostream>
#include <cstring>
#include <string>
#include <vector>
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
#include <pthread.h>

using namespace std;
#define NUM_THREADS 5

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
	if (send(sock, inputFromUserToSend.c_str(), inputFromUserToSend.size(), 0) == -1) {
		cout << "Couldn't send to server! Whoops!" << endl;
		return;
	}
}

bool sendFileToServerByChunks(const int& sock, string localTrainFile) {
	const int FILE_CHUNK_SIZE = 4095; 

	// Check if file exists
	ifstream file(localTrainFile, ios::binary);
	if(!file) {
		cout<<"invalid input" << endl;
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
		bytesSent += bytesToSend;

		response = returnResponseFromServer(sock);
		
		// if the server confirm to get the chunk
		if (response[0] == '1') {
			bytesSent += bytesToSend;
		} else if (response[0] == '2') {
			cout << "There was an error with the content of the file" << endl;
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
		cout << "There was an error with the content of the file" << endl;
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
		cout << returnResponseFromServer(sock);
		return;
	}

	string localTestFile;
	cout << "Please upload your local test CSV file." << endl;
	getline(cin, localTestFile);
	sendFileToServerByChunks(sock, localTestFile);
	cout << returnResponseFromServer(sock);
}


void algorithemSettings(const int& sock) {
	cout << returnResponseFromServer(sock);
	string userInput;
	getline(cin, userInput);
	sendToServer(sock, userInput);
	string response = returnResponseFromServer(sock);
	
	cout << response;
	if (response.find("Exit") == string::npos) {
		cout << returnResponseFromServer(sock);
	}
}

void classifyData(const int& sock) {
	// waiting for a response from server
	// Maayan implement : the server runs the CSV files from option 1
	// and server response back 'classifying data complete' or 'please upload data'
	// then get back to the menu
	string response = returnResponseFromServer(sock);
	
	cout << response;
	if (response.find("Exit") == string::npos) {
		cout << returnResponseFromServer(sock);
	}
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
		} else {
			if (!buf.empty() && buf[0] == '1') {
				sendToServer(sock, "1");
			} else {
				sendToServer(sock, "2");
				cout << "There was a problem receiving the content" << endl;
			}
			
			cout << returnResponseFromServer(sock);
			return;
		}
	} while(true);
}

void *PrintHello(void *threadid) {
   long tid;
   tid = (long)threadid;
   cout << "Hello World! Thread ID, " << tid << endl;
   pthread_exit(NULL);
}

// void *saveTheFile(void *threadid) {
// 	// here we ask from the user for a path to a file
// 	// we check if its exists, or we create it
// 	// and then we start to save all the chunks into the file
// }

void downloadResults(const int& sock) {
	// everytime the user enters a path to a file (find or create if doesn't exists)
	// we open a thread, so this thread actually uses a function, so that function
	// actually saves the content into this file (in chunks like operation 4).
	// in the meantime, the father thread doesn't wait to its child
	// and prints (as a receive from server) the menu again
	// if the user enters 5 again, and a path to a file, we open one more thread
	// so we have parallel threads, working both together

	// i need to check if it gives me new threads, or only 1
	pthread_t thread;
	cout << "creating thread " << &thread << endl;
	int rc = pthread_create(&thread, NULL, PrintHello, (void *)&thread);

	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}

	//pthread_exit(NULL);
	// pthread_t threads[NUM_THREADS];
   	// int rc;
   	// int i;
   
	// for( i = 0; i < NUM_THREADS; i++ ) {
	// 	cout << "creating thread, " << i << endl;
	// 	//rc = pthread_create(&threads[i], NULL, saveTheFile, (void *)i);
	// 	rc = pthread_create(&threads[i], NULL, PrintHello, (void *)&i);
		
	// 	if (rc) {
	// 		cout << "Error:unable to create thread," << rc << endl;
	// 		exit(-1);
	// 	}
	// }
	// pthread_exit(NULL);
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
	
	cout << returnResponseFromServer(sock);
	
    do {
		// after the menu, the user enters a number according to the menu options
        getline(cin, userInput);

		// If the client didn't enter a number
		if (!checkNumber(userInput)) {
			cout << "An invalid option" << endl;
			continue;
		}
		
		sendToServer(sock, userInput);

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
				downloadResults(sock);
				break;
			case 8:
				shouldExit = true;
				break;
			default:
				cout << "invalid input" << endl;
			}
    } while(!shouldExit);

    close(sock);
    return 0;
}
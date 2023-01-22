#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <list>

#include "Network/Server.h"
#include "ClientApp/ClientThread.h"
#include "Utils/ParseMethods.h"

using namespace std;
using namespace network;

int main(int numberOfArguments, char* arguments[]) {
	numberOfArguments--;
    if (numberOfArguments != 2) {
        cout << "Invalid number of command line arguments: " << numberOfArguments << "!\n";
        return 0;
    }
	
	int port;
	if (!app::ParseMethods::parse(arguments[2], port) || port > 65535 || port < 0) {
		cout << "Invalid port given: " << arguments[2] << endl;
		return 0;
	}
	
	Server server;

    try {
		server = Server(port);
    } catch (runtime_error exception) {
        cout << exception.what() << "\n";
        return 0;
    }

	list<thread> threads;

    while (true) {
		try {
			threads.push_back(thread(app::ClientThread(), server.AcceptNewClient()));
		} catch (runtime_error exception) {
			cout << exception.what() << "\n";
		}
    }

    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

#include <string.h>

#include "Server.h"
#include "Distances.h"
#include "LabeledDataSet.h"

using namespace std;
using namespace math;
using namespace data;
using namespace network;

bool parseClientData(const string& clientData, 
					 vector<double>& parsedVector,
					 DistanceFactory::DistanceType& parsedDistanceType,
					 int& k)
{
	stringstream stream(clientData);
	
    // Parse the vector
    do {
        double currentNumber;
        stream >> currentNumber;

        if (!stream.fail()) {
            parsedVector.push_back(currentNumber);
        }
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
	
	const unordered_map<string, DistanceFactory::DistanceType> conversionMap = {
        {"AUC", DistanceFactory::DistanceType::Euclidean},
        {"MAN", DistanceFactory::DistanceType::Manhattan},
        {"CHB", DistanceFactory::DistanceType::Chebyshev},
        {"CAN", DistanceFactory::DistanceType::Canberra},
        {"MIN", DistanceFactory::DistanceType::Minkowski}
    };

    if (conversionMap.find(distanceString) == conversionMap.cend()) { 
        return false;
    }

    parsedDistanceType = conversionMap.at(distanceString);
	
	// Parse k
    stream >> k;
	
	return !stream.fail();
}

/*
* Input:
*	integerString = The string to parse
*	parsedInteger = The integer to parse the string into
* Output:
*	Whether the parse succeeded
*/
bool parse(const string& integerString, int& parsedInteger) {
    stringstream stream(integerString);
    stream >> parsedInteger;

    return !stream.fail();
}

void SendToClient(const ClientSocket& client, const string& dataToSend) {
	try {
		client.Send(dataToSend);
	} catch (runtime_error exception) {
		cout << exception.what() << "\n";
	}
}

int main(int numberOfArguments, char* arguments[]) {
	numberOfArguments--;
    if (numberOfArguments != 2) {
        cout << "Invalid number of command line arguments: " << numberOfArguments << "!\n";
        return 0;
    }
	
	int port;
	if (!parse(arguments[2], port) || port > 65535 || port < 0) {
		cout << "Invalid port given: " << arguments[2] << endl;
		return 0;
	}

    LabeledDataSet labeledDataSet;
	Server server;

    try {
        labeledDataSet = LabeledDataSet(arguments[1]);
		server = Server(port);
    } catch (runtime_error exception) {
        cout << exception.what() << "\n";
        return 0;
    }

    while (true) {
		ClientSocket client;
		std::string dataReceived;

		try {
			client = server.AcceptNewClient();
			dataReceived = client.Receive();
		} catch (runtime_error exception) {
			cout << exception.what() << "\n";
		}
		
		while (!dataReceived.empty()) {	
			vector<double> vectorToCategorize;
			DistanceFactory::DistanceType distanceType;
			int k;
			
			if (parseClientData(dataReceived, vectorToCategorize, distanceType, k) &&
				vectorToCategorize.size() == labeledDataSet.featureSize() &&
				k >= 1 && k <= labeledDataSet.size()) {	
				std::unique_ptr<Distance> distance = DistanceFactory::createDistance(distanceType);
				
				try {
					SendToClient(client, labeledDataSet.predictLabel(vectorToCategorize, *distance, k));
				} catch (runtime_error exception) {
					SendToClient(client, "invalid input");
				}
			} else {
				SendToClient(client, "invalid input");
			}
			
			try {
				dataReceived = client.Receive();
			} catch (runtime_error exception) {
				cout << exception.what() << "\n";
				dataReceived.clear();
			}
		}
    }

    return 0;
}

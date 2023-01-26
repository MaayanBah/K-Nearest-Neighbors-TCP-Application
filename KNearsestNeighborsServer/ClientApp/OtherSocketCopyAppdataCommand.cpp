#include <string>

#include "ClientApp/OtherSocketCopyAppdataCommand.h"
#include "ClientApp/ClientThread.h"
#include "Utils/ParseMethods.h"

using namespace app;
using namespace std;

void OtherSocketCopyAppdataCommand::execute() {
    io.write("1");
    string response = client.receive();
    int portReceived;

    if (!ParseMethods::parse(response, portReceived)) {
        io.write("2");
        return;
    }
    
    ClientThread::ClientData parentClient(client.getIP(), portReceived);

    if (ClientThread::clientsAppdata.find(parentClient) == ClientThread::clientsAppdata.end()) {
        io.write("2");
        return;
    }

    io.write("1");
    appData = *ClientThread::clientsAppdata.at(parentClient);
}
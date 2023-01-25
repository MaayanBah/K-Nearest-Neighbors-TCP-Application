#include <string>

#include "ClientApp/OtherSocketCopyAppdataCommand.h"
#include "ClientApp/ClientThread.h"
#include "Utils/ParseMethods.h"

using namespace app;
using namespace std;

void OtherSocketCopyAppdataCommand::execute() {
    client.send("1");
    string response = client.receive();
    int portReceived;

    if (!ParseMethods::parse(response, portReceived)) {
        client.send("2");
        return;
    }
    
    ClientThread::ClientData parentClient(client.getIP(), portReceived);

    if (ClientThread::clientsAppdata.find(parentClient) == ClientThread::clientsAppdata.end()) {
        client.send("2");
        return;
    }

    client.send("1");
    appData = *ClientThread::clientsAppdata.at(parentClient);
}
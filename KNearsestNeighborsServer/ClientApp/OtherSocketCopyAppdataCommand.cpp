#include <string>

#include "ClientApp/OtherSocketCopyAppdataCommand.h"
#include "ClientApp/ClientThread.h"
#include "Utils/ParseMethods.h"

using namespace app;
using namespace std;

void OtherSocketCopyAppdataCommand::execute() {
    client.send(string(1, '1'));
    string response = client.receive();
    int portReceived;

    if (!ParseMethods::parse(response, portReceived)) {
        client.send(string(1, '2'));
        return;
    }

    appData = ClientThread::clientsAppdata.at(ClientThread::ClientData(client.getIP(), portReceived));
}
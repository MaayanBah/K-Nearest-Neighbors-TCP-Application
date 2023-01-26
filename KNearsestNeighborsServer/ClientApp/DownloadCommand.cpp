#include <string>

#include "ClientApp/DownloadCommand.h"
#include "Utils/ParseMethods.h"
#include "ClientThread.h"

using namespace app;
using namespace std;

void DownloadCommand::execute() {
    if (appData.dataset.size() == 0 || appData.unclassifiedData.empty()) {
        io.write("please upload data\n");
        return;
    }

    if (appData.classificationResult.empty()) {
        io.write("please classify the data\n");
        return;
    }

    io.write("1");
    string response = io.read();
    int portReceived;

    if (!ParseMethods::parse(response, portReceived)) {
        io.write("2");
        return;
    }

    ClientThread::clientsAppdata[ClientThread::ClientData(client.getIP(), portReceived)] = &appData;
    io.write("1");
    io.read();
}
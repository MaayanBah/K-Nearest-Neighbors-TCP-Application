
#include <stdexcept>
#include <iostream>

#include "ClientApp/IO.h"
#include "ClientApp/Command.h"
#include "ClientApp/UploadCommand.h"
#include "ClientApp/AlgorithmSettingsCommand.h"
#include "ClientApp/ClassifyDataCommand.h"
#include "ClientApp/DownloadCommand.h"
#include "ClientApp/ClientThread.h"
#include "ClientApp/AppData.h"
#include "Utils/ParseMethods.h"
#include "Dataset/LabeledDataSet.h"
#include "Dataset/Distances.h"

using namespace std;
using namespace app;

ClientThread::ClientThread() {
    // supportedCommands.push_back(...);
}

void ClientThread::sendMenu(const network::ClientSocket& client,
                            const std::vector<std::unique_ptr<Command>>& supportedCommands) {
	for (int index = 0; index < supportedCommands.size(); index++) {
        client.send(to_string(index + 1) + ". " + supportedCommands[index]->description() + "\n");
	}

    client.send(to_string(EXIT_OPTION) + ". Exit\n");
}

void ClientThread::operator()(network::ClientSocket client) {
    AppData appData;
    SocketIO socketIO(client);

    vector<unique_ptr<Command>> supportedCommands = {
        make_unique<UploadCommand>(socketIO, appData),
        make_unique<AlgorithmSettingsCommand>(socketIO, appData),
        make_unique<ClassifyDataCommand>(socketIO, appData),
        make_unique<DownloadCommand>("display results", socketIO, appData),
        make_unique<DownloadCommand>("download results", socketIO, appData),
    };

    string response;

    do {
        try {
            sendMenu(client, supportedCommands);
            response = client.receive();

            if (response.empty()) {
                break;
            }

            int userChoice;
            if (!parse(response, userChoice)) {
                client.send("invalid input\n");
                continue;
            }
            
            if (userChoice == EXIT_OPTION) {
                break;
            }

            if (userChoice < 1 || userChoice > supportedCommands.size()) {
                break;
            }

            supportedCommands[userChoice - 1]->execute();
        } catch (runtime_error exception) {
            cout << exception.what() << "\n";
        }
    } while (!response.empty());
}
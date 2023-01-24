
#include <stdexcept>
#include <iostream>
#include <memory>

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
    string menu;
	for (int index = 0; index < supportedCommands.size(); index++) {
        menu += to_string(index + 1) + ". " + supportedCommands[index]->description() + "\n";
    }

    menu += to_string(EXIT_OPTION) + ". Exit\n";

    client.send(menu);
}

void ClientThread::operator()(network::ClientSocket client) {
    AppData appData;
    SocketIO socketIO(client);

    vector<unique_ptr<Command>> supportedCommands;
    supportedCommands.push_back(unique_ptr<UploadCommand>(new UploadCommand(socketIO, appData)));
    supportedCommands.push_back(unique_ptr<AlgorithmSettingsCommand>(new AlgorithmSettingsCommand(socketIO, appData)));
    supportedCommands.push_back(unique_ptr<ClassifyDataCommand>(new ClassifyDataCommand(socketIO, appData)));
    supportedCommands.push_back(unique_ptr<DownloadCommand>(new DownloadCommand("display results", socketIO, appData)));
    supportedCommands.push_back(unique_ptr<DownloadCommand>(new DownloadCommand("download results", socketIO, appData)));

    string response;

    try {
        do {
            sendMenu(client, supportedCommands);
            response = client.receive();

            int userChoice;
            if (ParseMethods::parse(response, userChoice)) {
                if (userChoice == EXIT_OPTION) {
                    break;
                }

                if (userChoice < 1 || userChoice > supportedCommands.size()) {
                    break;
                }

                supportedCommands[userChoice - 1]->execute();
            }
        } while (true);
    } catch (runtime_error exception) { }
}
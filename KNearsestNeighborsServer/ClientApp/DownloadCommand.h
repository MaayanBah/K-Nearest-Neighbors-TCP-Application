#ifndef __DOWNLOAD_COMMAND_H
#define __DOWNLOAD_COMMAND_H

#include "ClientApp/Command.h"
#include "ClientApp/AppData.h"
#include "Network/ClientSocket.h"

namespace app {
    class DownloadCommand : public Command {
    private:
        // Members
        AppData& appData;
        const network::ClientSocket& client;
        SocketIO io;

    public:
        // C'tor
        DownloadCommand(const network::ClientSocket& client, 
                        AppData& appData) :
            Command("download results", io),
            appData(appData),
            client(client),
            io(client) {
        }

        // Implemented Abstract Methods
        void execute() override;
    };
}

#endif
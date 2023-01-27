#ifndef __OTHER_SOCKET_DOWNLOAD_COMMAND_H
#define __OTHER_SOCKET_DOWNLOAD_COMMAND_H

#include "ClientApp/Command.h"
#include "ClientApp/AppData.h"

namespace app {
    class OtherSocketCopyAppdataCommand : public Command {
    private:
        // Members
        AppData& appData;
        const network::ClientSocket& client;
        SocketIO io;

    public:
        // C'tor
        OtherSocketCopyAppdataCommand(const network::ClientSocket& client,
                                   AppData& appData) :
            Command("download results", io),
            client(client),
            io(client),
            appData(appData) {
        }

        // Implemented Abstract Methods
        void execute() override;
    };
}

#endif
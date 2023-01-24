#ifndef __DOWNLOAD_COMMAND_H
#define __DOWNLOAD_COMMAND_H

#include "ClientApp/Command.h"
#include "ClientApp/AppData.h"

namespace app {
    class DownloadCommand : public Command {
    public:
        // C'tor
        DownloadCommand(DefaultIO& io) :
            Command("download results", io) {
        }

        // Implemented Abstract Methods
        void execute() override { }
    };
}

#endif
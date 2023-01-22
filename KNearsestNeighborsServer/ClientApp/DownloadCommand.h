#ifndef __DOWNLOAD_COMMAND_H
#define __DOWNLOAD_COMMAND_H

#include "ClientApp/Command.h"
#include "ClientApp/AppData.h"

namespace app {
    class DownloadCommand : public Command {
    private:
        // Members
        AppData& appData;

    public:
        // C'tor
        DownloadCommand(std::string description,
                        DefaultIO& io,
                        AppData& appData) :
            Command(std::move(description), io),
            appData(appData) {
        }

        // Implemented Abstract Methods
        void execute() override;
    };
}

#endif
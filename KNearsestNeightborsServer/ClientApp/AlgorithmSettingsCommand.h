#ifndef __ALGORITHM_SETTINGS_COMMAND_H
#define __ALGORITHM_SETTINGS_COMMAND_H

#include "ClientApp/Command.h"
#include "ClientApp/AppData.h"
#include "Dataset/Distances.h"

namespace app {
    class AlgorithmSettingsCommand : public Command {
    private:
        // Members
        DefaultIO& io;
        AppData& appData;

    public:
        // C'tor
        AlgorithmSettingsCommand(DefaultIO& io, 
                                 AppData& appData) :
            Command("algorithm settings"),
            io(io),
            appData(appData) {
        }

        // Implemented Abstract Methods
        void execute() override;
    };
}

#endif
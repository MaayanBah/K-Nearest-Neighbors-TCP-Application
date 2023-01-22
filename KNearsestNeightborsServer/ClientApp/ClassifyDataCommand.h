#ifndef __CLASSIFY_DATA_COMMAND_H
#define __CLASSIFY_DATA_COMMAND_H

#include "ClientApp/Command.h"
#include "ClientApp/AppData.h"

namespace app {
    class ClassifyDataCommand : public Command {
    private:
        // Members
        AppData& appData;

    public:
        // C'tor
        ClassifyDataCommand(DefaultIO& io,
                            AppData& appData) :
            Command("classify data", io),
            appData(appData) {
        }

        // Implemented Abstract Methods
        void execute() override;
    };
}

#endif
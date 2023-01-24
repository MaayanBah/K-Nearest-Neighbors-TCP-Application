#ifndef __DISPLAY_COMMAND_H
#define __DISPLAY_COMMAND_H

#include "ClientApp/Command.h"
#include "ClientApp/AppData.h"

namespace app {
    class DisplayCommand : public Command {
    private:
        // Members
        AppData& appData;

    public:
        // C'tor
        DisplayCommand(DefaultIO& io,
                       AppData& appData) :
            Command("display results", io),
            appData(appData) {
        }

        // Implemented Abstract Methods
        void execute() override;
    };
}

#endif
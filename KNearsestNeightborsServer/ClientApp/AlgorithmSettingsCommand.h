#ifndef __ALGORITHM_SETTINGS_COMMAND_H
#define __ALGORITHM_SETTINGS_COMMAND_H

#include "ClientApp/Command.h"
#include "Dataset/Distances.h"

namespace app {
    class AlgorithmSettingsCommand : public Command {
    private:
        // Members
        DefaultIO& io;
        data::LabeledDataSet& dataSet;
        int& k;
        math::DistanceFactory::DistanceType& distanceMethod;

    public:
        // C'tor
        AlgorithmSettingsCommand(DefaultIO& io, 
                                 data::LabeledDataSet& dataSet,
                                 int& k,
                                 math::DistanceFactory::DistanceType& distanceMethod) :
            Command("algorithm settings"),
            io(io),
            dataSet(dataSet),
            k(k),
            distanceMethod(distanceMethod) {
        }

        // Implemented Abstract Methods
        void execute() override;
    };
}

#endif

#include "Dataset/LabeledDataSet.h"
#include "Dataset/Distances.h"

#include <vector>

namespace app
{
    struct AppData {
        // Substructs
        struct Result {
            int unclassifiedDataIndex;
            std::string label;
        };

        // Members
        data::LabeledDataSet dataset;
        int k;
        math::DistanceFactory::DistanceType distanceMethod;
        std::vector<std::vector<double>> unclassifiedData;
        std::vector<Result> results;

        // C'tor
        AppData() : 
            k(5),
            distanceMethod(math::DistanceFactory::DistanceType::Euclidean) {
        }
    };
}
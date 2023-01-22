
#include "Dataset/LabeledDataSet.h"
#include "Dataset/Distances.h"

#include <vector>

namespace app
{
    struct AppData {
        // Members
        data::LabeledDataSet dataset;
        int k;
        math::DistanceFactory::DistanceType distanceMethod;
        std::vector<std::vector<double>> unclassifiedData;
        std::string classificationResult;

        // C'tor
        AppData() : 
            k(5),
            distanceMethod(math::DistanceFactory::DistanceType::Euclidean) {
        }
    };
}
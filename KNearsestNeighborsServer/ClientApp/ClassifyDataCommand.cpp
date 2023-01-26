#include <vector>
#include <memory>

#include "ClientApp/ClassifyDataCommand.h"
#include "Dataset/Distances.h"

using namespace app;
using namespace std;
using namespace math;

void ClassifyDataCommand::execute() {
    if (appData.dataset.size() == 0 || appData.unclassifiedData.empty()) {
        io.write("please upload data\n");
        return;
    }

    appData.classificationResult.clear();
    unique_ptr<Distance> distance = DistanceFactory::createDistance(appData.distanceMethod);

    int vectorIndex = 0;
    for (const vector<double>& unclassifiedVector : appData.unclassifiedData) {
        appData.classificationResult += 
            (to_string(++vectorIndex) + " " + 
             appData.dataset.predictLabel(unclassifiedVector,
                                          *distance, 
                                          appData.k)) + "\n";
    }

    appData.classificationResult += "Done.\n";
    io.write("classifying data complete\n");
}
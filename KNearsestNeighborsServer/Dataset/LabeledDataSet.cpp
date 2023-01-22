#include "Dataset/LabeledDataSet.h"
#include <iostream>
#include <fstream>
#include <sstream> 
#include <algorithm>
#include <unordered_map>
#include <utility>

using namespace std;
using namespace math;

/*
* Input:
*   classifiedFilePath = The file path of the classified data.
* Output:
*   This builds the labeled dataset by the file passed.
*/
data::LabeledDataSet::LabeledDataSet(std::vector<LabeledData> dataSet) : 
    dataSet(std::move(dataSet)) {
}

/*
* Input:
*   vectorToCategorize = The input vector
*   distanceMethod = the method of distance to calculate with
*   k = in order to categorize the vector, we use K "closest" vectors. k is an input from user
* Output:
*   This builds the vector of distances, by a specific distance method.
*/
std::string data::LabeledDataSet::predictLabel(const std::vector<double> vectorToCategorize,
                                               Distance& distanceMethod,
                                               int k) const {
    vector<pair<string, double>> labeledDistances;
    labeledDistances.reserve(dataSet.size());

    // Calculates the distance between our input vector 
    // to each line (vector) of classified file, and saves its category 
    for (const LabeledData& item : dataSet) {
        labeledDistances.push_back(make_pair(item.label,distanceMethod.calculate(item.features, vectorToCategorize)));
    }

    // Sort the distance N vector, by its distance
    sort(labeledDistances.begin(), 
         labeledDistances.end(), 
         [](const pair<string, double>& leftLabeledDistance, 
            const pair<string, double>& rightLabeledDistance) { return leftLabeledDistance.second < 
                                                                       rightLabeledDistance.second; });

    // Create a map to count the appearances of a string (category) in labeledDistances vector
    unordered_map<string, int> labelCounters;

    for (int labeledDistanceIndex = 0; labeledDistanceIndex < k; labeledDistanceIndex++) {
        const string& currentLabel = labeledDistances[labeledDistanceIndex].first;

        if (labelCounters.find(currentLabel) != labelCounters.end()) {
            labelCounters.at(currentLabel)++;
        } else {
            labelCounters.insert(make_pair(currentLabel, 1));
        }
    }

    return max_element(labelCounters.begin(), 
                       labelCounters.end(), 
                       [](const std::pair<string, int>& leftLabelCounter, 
                          const std::pair<string, int>& rightLabelCounter){
                           return leftLabelCounter.second < rightLabelCounter.second;
                       })->first;
}
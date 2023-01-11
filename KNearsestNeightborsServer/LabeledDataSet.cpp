#include "LabeledDataSet.h"
#include <iostream>
#include <fstream>
#include <sstream> 
#include <algorithm>
#include <unordered_map>

using namespace std;
using namespace math;

/*
* Input:
*   classifiedFilePath = The file path of the classified data.
* Output:
*   This builds the labeled dataset by the file passed.
*/
data::LabeledDataSet::LabeledDataSet(const string& classifiedFilePath) {
    ifstream classifiedFile(classifiedFilePath);

    if (!classifiedFile.is_open()) {
        throw runtime_error("Failed opening classified file!");
    }

    string line;
    
    // Go over the file line by line
    while (getline(classifiedFile, line)) {
        LabeledData currentLabeledData(line);

        // If the parsed labeled data does not match the previous element of the dataset in feature length
        if (!dataSet.empty() && dataSet[dataSet.size() - 1].features.size() != currentLabeledData.features.size()) {
            throw runtime_error("The size of the features isn't consistent!");
        }

        dataSet.push_back(std::move(currentLabeledData));
    }

    classifiedFile.close();
}

/*
* Input:
*   line = The line that represents the labeled data
* Output:
*   This builds the LabeledData by the line passed.
*/
data::LabeledDataSet::LabeledData::LabeledData(const std::string& line) {
    stringstream stream(line);

    // Go over the line comma after comma, until we reach the end, or we've failed, or we've parsed the label
    while (!stream.eof() && !stream.fail() && label.empty()) {
        string currentString;
        getline(stream, currentString, ',');

        stringstream doubleStream(currentString);
        double currentNumber;
        doubleStream >> currentNumber;
        
        // If the current element is not a double, it's the label.
        if (doubleStream.fail()) {
            label = currentString;
        } else {
            features.push_back(currentNumber);
        }
    }

    if (!stream.eof() || stream.fail()) {
        throw runtime_error("Failed parsing the line!");
    }

    if (features.empty()) {
        throw runtime_error("The line parsed contained no features!");
    }

    if (label.empty()) {
        throw runtime_error("The line parsed contained no label!");
    }
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
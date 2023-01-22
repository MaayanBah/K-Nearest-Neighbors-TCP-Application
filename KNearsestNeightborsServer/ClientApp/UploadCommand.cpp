#include <vector>
#include <sstream>

#include "ClientApp/UploadCommand.h"
#include "Utils/ParseMethods.h"

using namespace app;
using namespace std;

std::string UploadCommand::getFileFromIO() {
    string uploadedData;
    string currentPacketReceived = io.read();

    while (currentPacketReceived[0] != FINISHED_MARK) {
        if (currentPacketReceived[0] != DATA_MARK) {
            io.write(to_string(ERROR_MARK));
            break;
        }

        uploadedData += currentPacketReceived.substr(1);
        io.write(to_string(FINISHED_MARK));
        currentPacketReceived = io.read();
    }

    return uploadedData;
}

bool UploadCommand::parseClassifiedFile(const std::string& classifiedFile, 
                                        std::vector<data::LabeledDataSet::LabeledData>& classifiedLabels) {
    string line;
    stringstream classifiedFileStream(classifiedFile);
    
    // Go over the file line by line
    while (getline(classifiedFileStream, line)) {
        data::LabeledDataSet::LabeledData currentLabeledData;
        if (!parseLabeledData(line, currentLabeledData)) {
            return false;
        }

        // If the parsed labeled data does not match the previous element of the dataset in feature length
        if (!classifiedLabels.empty() && classifiedLabels[classifiedLabels.size() - 1].features.size() != currentLabeledData.features.size()) {
            return false;
        }

        classifiedLabels.push_back(std::move(currentLabeledData));
    }

    return true;
}

bool UploadCommand::parseLabeledData(const std::string& line,
                                     data::LabeledDataSet::LabeledData& labeledData) {
    stringstream stream(line);

    // Go over the line comma after comma, until we reach the end, or we've failed, or we've parsed the label
    while (!stream.eof() && !stream.fail() && labeledData.label.empty()) {
        string currentString;
        getline(stream, currentString, ',');

        stringstream doubleStream(currentString);
        double currentNumber;
        doubleStream >> currentNumber;
        
        // If the current element is not a double, it's the label.
        if (doubleStream.fail()) {
            labeledData.label = currentString;
        } else {
            labeledData.features.push_back(currentNumber);
        }
    }

    return stream.eof() && !stream.fail() && !labeledData.features.empty() && !labeledData.label.empty();
}

bool UploadCommand::parseUnclassifiedFile(const std::string& unclassifiedFile,
                                          std::vector<std::vector<double>>& unclassifiedVectors,
                                          int expectedVectorSize) {
    
    string line;
    stringstream unClassifiedFileStream(unclassifiedFile);
    
    // Go over the file line by line
    while (getline(unClassifiedFileStream, line)) {
        vector<double> unclassifiedVector;
        if (!parse(line, unclassifiedVector)) {
            return false;
        }

        if (unclassifiedVector.size() != expectedVectorSize) {
            return false;
        }

        unclassifiedVectors.push_back(std::move(unclassifiedVector));
    }

    return true;
}

void UploadCommand::execute() {
    string classifiedFile = getFileFromIO();

    vector<data::LabeledDataSet::LabeledData> classifiedLabels;
    if (!parseClassifiedFile(classifiedFile, classifiedLabels) || classifiedLabels.size() < appData.k) {
        io.write(to_string(ERROR_MARK));
        return;
    }

    io.write("Upload complete.\n");
    string unclassifiedFile = getFileFromIO();

    vector<vector<double>> unclassifiedVectors;
    if (!parseUnclassifiedFile(unclassifiedFile, unclassifiedVectors, classifiedLabels[0].features.size()) || unclassifiedVectors.empty()) {
        io.write(to_string(ERROR_MARK));
        return;
    }

    io.write("Upload complete.\n");

    appData.dataset = data::LabeledDataSet(std::move(classifiedLabels));
    appData.unclassifiedData = std::move(unclassifiedVectors);
}

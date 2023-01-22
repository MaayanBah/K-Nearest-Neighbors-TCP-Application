#include <sstream>
#include <unordered_map>

#include "ClientApp/AlgorithmSettingsCommand.h"
#include "Utils/ParseMethods.h"

using namespace app;
using namespace std;
using namespace math;

void AlgorithmSettingsCommand::execute() {
    io.write(string("The current KNN parameters are: K = ") + 
             to_string(appData.k) + 
             ", distance metric = " +
             DistanceFactory::distanceName(appData.distanceMethod) + "\n");
    
    std::string response = io.read();
    if (response == "\n") {
        return;
    }

    string badIntegerMessage = "invalid value for K\n";
    string badDistanceMessage = "invalid value for metric\n";

    stringstream stream(response);

    string kString;
    stream >> kString;

    if (stream.fail()) {
        io.write(badIntegerMessage + badDistanceMessage);
        return;
    }

    int newK;
    bool wasKParsed = ParseMethods::parse(kString, newK) && newK >= 1 && (appData.dataset.size() == 0 || newK <= appData.dataset.size());
    if (!wasKParsed) {
        io.write(badIntegerMessage);
    }

	std::string distanceString;
	stream >> distanceString;
	
	if (stream.fail()) {
        io.write(badDistanceMessage);
		return;
	}	
	
	const unordered_map<string, DistanceFactory::DistanceType> conversionMap = {
        {"AUC", DistanceFactory::DistanceType::Euclidean},
        {"MAN", DistanceFactory::DistanceType::Manhattan},
        {"CHB", DistanceFactory::DistanceType::Chebyshev},
        {"CAN", DistanceFactory::DistanceType::Canberra},
        {"MIN", DistanceFactory::DistanceType::Minkowski}
    };

    if (conversionMap.find(distanceString) == conversionMap.cend()) {
        io.write(badDistanceMessage);
        return;
    }

    if (wasKParsed) {
        appData.k = newK;
        appData.distanceMethod = conversionMap.at(distanceString);
    }
}
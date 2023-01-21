#include <sstream>
#include <unordered_map>

#include "Commands/AlgorithmSettingsCommand.h"
#include "ParseMethods.h"

using namespace app;
using namespace std;
using namespace math;

void AlgorithmSettingsCommand::execute() {
    io.write(string("The current KNN parameters are: K = ") + 
             to_string(k) + 
             ", distance metric = " +
             DistanceFactory::distanceName(distanceMethod) + "\n");
    
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
    bool wasKParsed = parse(kString, newK) && newK >= 1 && newK <= dataSet.size();
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
        k = newK;
        distanceMethod = conversionMap.at(distanceString);
    }
}

#include <sstream>
#include "Utils/ParseMethods.h"

using namespace app;
using namespace std;

/*
* Input:
*	integerString = The string to parse
*	parsedInteger = The integer to parse the string into
* Output:
*	Whether the parse succeeded
*/
bool parse(const string& integerString, int& parsedInteger) {
    stringstream stream(integerString);
    stream >> parsedInteger;

    return !stream.fail();
}

/*
* Input:
*	doublesString = The string to parse
*	parsedDoubles = The doubles to parse the string into
* Output:
*	Whether the parse succeeded
*/
bool parse(const std::string& doublesString, vector<double>& parsedDoubles) {
    stringstream stream(doublesString);

    while (!stream.eof() && !stream.fail()) {
        string currentString;
        getline(stream, currentString, ',');

        stringstream doubleStream(currentString);
        double currentNumber;
        doubleStream >> currentNumber;
        
        if (doubleStream.fail()) {
            return false;
        }
        
        parsedDoubles.push_back(currentNumber);
    }

    return true;
}

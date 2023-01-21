
#include <sstream>
#include "ParseMethods.h"

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
#ifndef __PARSE_METHODS_H
#define __PARSE_METHODS_H

#include <string>
#include <vector>

namespace app {
    bool parse(const std::string& stringToParse, int& parsedInt);
    bool parse(const std::string& stringToParse, vector<double>& parsedDoubles);
}

#endif

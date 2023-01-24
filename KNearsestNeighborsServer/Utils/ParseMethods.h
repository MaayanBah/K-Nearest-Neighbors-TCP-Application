#ifndef __PARSE_METHODS_H
#define __PARSE_METHODS_H

#include <string>
#include <vector>

namespace app {
    class ParseMethods {
    public:
        static bool parse(const std::string& stringToParse, int& parsedInt);
        static bool parse(const std::string& stringToParse, std::vector<double>& parsedDoubles);
    };
}

#endif

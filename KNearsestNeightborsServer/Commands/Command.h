#ifndef __COMMAND_H
#define __COMMAND_H

#include <string>

#include "Commands/IO.h"
#include "LabeledDataSet.h"

namespace app {
    class Command {
    private:
        // Members
        std::string descriptionMember;

    public:
        // C'tor
        Command(std::string description) : 
            descriptionMember(description) {
        }

        // D'tor
        virtual ~Command() = default;

        // Getter Methods
        inline const std::string& description() const { return descriptionMember; }

        // Virtual Methods
        virtual void execute() = 0;
    };
}

#endif
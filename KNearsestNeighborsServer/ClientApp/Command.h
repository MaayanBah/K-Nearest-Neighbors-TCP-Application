#ifndef __COMMAND_H
#define __COMMAND_H

#include <string>

#include "ClientApp/IO.h"
#include "Dataset/LabeledDataSet.h"

namespace app {
    class Command {
    private:
        // Members
        std::string descriptionMember;
    
    protected:
        // Members
        DefaultIO& io;

    public:
        // C'tor
        Command(std::string description, DefaultIO& io) : 
            descriptionMember(std::move(description)),
            io(io) {
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
#ifndef __UPLOAD_COMMAND_H
#define __UPLOAD_COMMAND_H

#include <string>

#include "ClientApp/Command.h"
#include "ClientApp/AppData.h"

namespace app {
    class UploadCommand : public Command {
    private:
        // Consts
        static constexpr char DATA_MARK = '0';
        static constexpr char FINISHED_MARK = '1';
        static constexpr char ERROR_MARK = '2';

        // Members
        AppData& appData;

        // Protocol Methods
        std::string getFileFromIO();

        // Parsing Methods
        static bool parseClassifiedFile(const std::string& classifiedFile, 
                                        std::vector<data::LabeledDataSet::LabeledData>& classifiedLabels);
        static bool parseLabeledData(const std::string& line,
                                     data::LabeledDataSet::LabeledData& labeledData);
        static bool parseUnclassifiedFile(const std::string& unclassifiedFile, 
                                          std::vector<std::vector<double>>& unclassifiedVectors,
                                          int expectedVectorSize);

    public:
        // C'tor
        UploadCommand(DefaultIO& io,
                      AppData& appData) :
            Command("upload an unclassified csv data file", io),
            appData(appData) {
        }

        // Implemented Abstract Methods
        void execute() override;
    };
}

#endif
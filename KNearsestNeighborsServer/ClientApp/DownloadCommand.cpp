#include <vector>

#include "ClientApp/DownloadCommand.h"

using namespace app;
using namespace std;

void DownloadCommand::execute() {
    if (appData.dataset.size() == 0 || appData.unclassifiedData.empty()) {
        io.write("please upload data\n");
        return;
    }

    if (appData.classificationResult.empty()) {
        io.write("please classify the data\n");
        return;
    }

    int sizeToSend;

    for (int currentIndex = 0; 
         currentIndex < appData.classificationResult.size();
         currentIndex += sizeToSend) {
        sizeToSend = min<int>(4096, appData.classificationResult.size() - currentIndex);
        io.write(string(1, '0') + appData.classificationResult.substr(currentIndex, sizeToSend));

        if (io.read()[0] != '1') {
            break;
        }
    }

    io.write(string(1, '1'));
    io.read();
}

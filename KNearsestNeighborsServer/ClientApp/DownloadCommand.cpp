#include <vector>

#include "ClientApp/DownloadCommand.h"

using namespace app;
using namespace std;

void DownloadCommand::execute() {
    int sizeToSend;

    for (int currentIndex = 0; 
         currentIndex < appData.classificationResult.size();
         currentIndex += sizeToSend) {
        sizeToSend = min<int>(4096, appData.classificationResult.size() - currentIndex);
        io.write(to_string('0') + appData.classificationResult.substr(currentIndex, sizeToSend));

        if (io.read()[0] != '1') {
            break;
        }
    }

    io.write(to_string('1'));
    io.read();
}

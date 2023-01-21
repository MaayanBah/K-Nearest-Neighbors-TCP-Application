#include <string>
#include <iostream>

#include "Network/ClientSocket.h"

namespace app {
    class DefaultIO {
    public:
        // D'tor
        virtual ~DefaultIO() = default;

        // Abstract Methods
        virtual void write(const std::string& dataToWrite) = 0;
        virtual std::string read() = 0;
    };

    class SocketIO : public DefaultIO {
    private:
        // Members
        const network::ClientSocket& client;

    public:
        // C'tor
        SocketIO(const network::ClientSocket& clientSocket) :
            client(clientSocket) {

        }

        // Methods
        void write(const std::string& dataToWrite) override {
            client.send(dataToWrite);
        }

        std::string read() override {
            return client.receive();
        }
    };

    class StandardIO : public DefaultIO {
    public:
        // Methods
        void write(const std::string& dataToWrite) override {
            std::cout << dataToWrite;
        }

        std::string read() override {
            std::string output;
            std::cin >> output;
            return output;
        }
    };
}
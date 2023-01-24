#ifndef __CLIENT_THREAD_H
#define __CLIENT_THREAD_H

#include "Network/ClientSocket.h"
#include "ClientApp/Command.h"
#include "ClientApp/AppData.h"

#include <utility>
#include <vector>
#include <memory>
#include <map>

namespace app
{
    class ClientThread
    {
    private:
        // Consts
        static const int EXIT_OPTION = 8;

        // Methods
        void sendMenu(const network::ClientSocket& client,
                      const std::vector<std::unique_ptr<Command>>& supportedCommands);

    public:
        // Substructs
        struct ClientData {
            inline bool operator<(const ClientData& other) const {
                return ip < other.ip || (ip == other.ip && port < other.port);
            }

            ClientData(const network::ClientSocket& sock) : 
                ip(sock.getIP()),
                port(sock.getPort()) { }

            ClientData(unsigned int ip, unsigned short port) : 
                ip(ip),
                port(port) { }

            unsigned int ip;
            unsigned short port;
        };

        // AppData Mapping
        static std::map<ClientData, AppData> clientsAppdata;

        // Thread Method
        void operator()(network::ClientSocket client);
    };
}

#endif
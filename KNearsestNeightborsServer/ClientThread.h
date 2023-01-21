#ifndef __CLIENT_THREAD_H
#define __CLIENT_THREAD_H

#include "ClientSocket.h"
#include <utility>

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
        // C'tor
        ClientThread();

        // Thread Method
        void operator()(network::ClientSocket client);
    };
}

#endif
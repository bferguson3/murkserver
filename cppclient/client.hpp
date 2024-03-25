// client.hpp
#ifndef __MURKCLIENT_H__
#define __MURKCLIENT_H__

#include <enet/enet.h>
#include <termios.h>

namespace Murk { 

class Client {

    public:

        void InitEnet();
        void Connect(ENetAddress* address);
        void ProcessEvent(ENetEvent* event);
        void Disconnect();
        void SetNonblocking();

        ENetHost* host;
        ENetPeer* server;
        char id[16];

    private:

        struct termios old_terminal_settings;
        void err(int i, const char* s);

};

}

#endif 
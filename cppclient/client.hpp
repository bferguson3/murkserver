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
        void ProcessInput(char* i);

        ENetHost* host;
        ENetPeer* server;
        char id[16];
        
        int input_ctr;

    private:

        struct termios old_terminal_settings;
        void err(int i, const char* s);
        
        char last_c = 0;
        bool arrow_check = false;
        bool arrow_get = false;
};

}

#endif 
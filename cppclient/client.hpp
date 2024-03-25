// client.hpp
#ifndef __MURKCLIENT_H__
#define __MURKCLIENT_H__

#include <enet/enet.h>
#include <termios.h>
#include <string>

extern "C"
{
    #include "murk.h"
}

namespace Murk { 


struct UserPass { 
    std::string user;
    std::string pass;
};


class Client {

    public:
        Client();
        void InitEnet();
        void Connect(ENetAddress* address);
        void ProcessEvent(ENetEvent* event);
        void Disconnect();
        void SetNonblocking();
        void ProcessInput(char* i);

        void GameLoop();

        ENetHost* host;
        ENetPeer* server;
        char id[16];
        struct UserPass userpass;

        int input_ctr;

        enum UserState state;

    private:

        struct termios old_terminal_settings;
        void err(int i, const char* s);
        
        char last_c;// = 0;
        bool arrow_check;// = false;
        bool arrow_get;// = false;

        // flags
        bool FLAG_INPUT_USER;
        bool FLAG_INPUT_PASSWORD;
};

}

#endif 
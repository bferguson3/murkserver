// client.hpp
#ifndef __MURKCLIENT_H__
#define __MURKCLIENT_H__

#include <enet/enet.h>
#include <termios.h>
#include <string>
#include <cstdio>
#include <fcntl.h>
#include "screen.hpp"
#include "packet.hpp"
#include <time.h>

#include "verb_context.h"

#define _CPP_

extern "C"
{
    #include "../murk.h"
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
        void ProcessPacket(Murk::Packet p);
        void Disconnect();
        void SetNonblocking();
        void ProcessInput(char* i);
        void AnsiPrint(std::string s);
        std::string InputPrompt();
        void Capitalize(char* input);
        void CapitalizeVerb(std::string* s);

        void SendSayMessage(std::string msg); // to current screen

        void GameLoop();
        void SendLogin();
        void SendMenuSelect(char s);
        void SendCommand(std::string s);
        void* Encrypt(const char* dat);

        ENetHost* host;
        ENetPeer* server;
        char            id[16];
        struct UserPass userpass;
        std::string last_cmd;

        int input_ctr;

        enum PlayerState state;
        Screen           currentScreen;

        struct timespec last_action_time;

    // PUBLIC VARS FROM SERVER 
#include "../cppserver/character_data.h"

    private:

        struct termios old_terminal_settings;
        void err(int i, const char* s);
        
        char last_c;// = 0;
        bool arrow_check;// = false;
        bool arrow_get;// = false;

        // flags
        bool FLAG_INPUT_USER;
        bool FLAG_INPUT_PASSWORD;
        bool FLAG_INPUT_MENU;
        bool FLAG_MUTE_INPUT;

        std::string verb_context;
        //std::vector<std::string> verb_context;
        std::vector<Object> object_context;
};

}

#endif 
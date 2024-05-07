#ifndef __SERVER_HPP__
#define __SERVER_HPP__

extern "C"
{
  #include <enet/enet.h>
  #include "../murk.h"
}
#include <iostream>
#include <sqlite3.h>
#include "user.hpp"
#include "guid.hpp"
#include <vector>
#include <unordered_map>
#include "../cppclient/packet.hpp"


using namespace std;

namespace Murk { 

class Server {

    public:
    
        void Init();
        void ProcessEvent(ENetEvent event);
        void DeInit();
        void ProcessPacket(Packet p);
        bool CheckPassword(std::string u, std::string p);

        // TODO; make a template 
        void SendLocalMessage(std::string a);
        void SendLocalMessage(std::string a, std::string b);
        void SendLocalMessage(void* s, std::string a, std::string b, std::string c);
        void SendLocalMessage(std::string a, std::string b, std::string c, std::string d);
        void SendLocalMessage(std::string a, std::string b, std::string c, std::string d, std::string e);

        ENetHost*   server;
        ENetAddress address;
        sqlite3*    murk_userdb;

        void AddScreen(Screen s);

        
        //std::vector <MurkUser> activeUsers;
        std::unordered_map <std::string, Murk::User> activeUserMap; // guid, User object
        std::vector<Murk::Screen> screensList;


    private:

        void InitEnet();
        void InitSQL();
        static int pw_callback(void *_notused, int argc, char **argv, char **azColName);

#define NUM_ACTIVE_USERS 1000
//        const int NUM_ACTIVE_USERS;// = 1000;

};

}

#endif 
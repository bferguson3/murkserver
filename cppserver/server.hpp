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

using namespace std;


class MurkServer {

    public:
    
        void InitEnet();
        void InitSQL();
        void ProcessEvent(ENetEvent event);

        ENetHost* server;
        ENetAddress address;
        sqlite3* murk_userdb;

        //std::vector <MurkUser> activeUsers;
        std::unordered_map <std::string, MurkUser> activeUserMap;

    private:
        const int NUM_ACTIVE_USERS = 1000;

};
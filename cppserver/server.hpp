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
#include "packet.hpp"


using namespace std;

namespace Murk { 

class Server {

    public:
    
        void Init();
        void ProcessEvent(ENetEvent event);
        void DeInit();
        void ProcessPacket(Packet p);

        ENetHost* server;
        ENetAddress address;
        sqlite3* murk_userdb;

        //std::vector <MurkUser> activeUsers;
        std::unordered_map <std::string, Murk::User> activeUserMap;

    private:

        void InitEnet();
        void InitSQL();

        const int NUM_ACTIVE_USERS = 1000;

};

}
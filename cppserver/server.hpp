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
/*
struct Key { 
    std::string guid;

    bool operator==(const Key &other) const { 
      return !(strcmp(guid.c_str(),other.guid.c_str()));
    }
};

template<>
struct std::hash<Key>{ 
  std::size_t operator()(const Key& k) const { 
    return hash<string>()(k.guid);
  }
};
*/

namespace Murk { 


class Server {

    public:
    
        void Init();
        void ProcessEvent(ENetEvent event);
        void DeInit();
        void ProcessPacket(Packet p);
        bool CheckPassword(std::string u, std::string p);

        void SendLocalMessage(void* s, std::string a);
        
        User GetUserFromActiveUserMap(std::string id);

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
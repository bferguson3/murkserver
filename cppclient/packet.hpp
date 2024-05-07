//packet.hpp
#ifndef __PACKET_HPP__
#define __PACKET_HPP__

#include <string>
#include <map>
#include <set>
#include "../cppserver/screen.hpp"

extern "C" {
  #include "../murk.h"
  #include <enet/enet.h>
}

namespace Murk
{

class Packet 
{

    public:
        Packet();
        Packet(enum MURK_PACKET_TYPES e);
        Packet(const char* s);
        
        int   Validate();     // Ensures str is a valid json blob 
        void  ParseData();   // Stores payload in data map 
        
        std::string GetString();        // returns the json blob 
        void        SetString(const char* s);  // sets the json blob 
        void SetMessage(std::string msg);
        void SetScreen(int id);

        ENetPeer*   GetPeer();
        void        SetPeer(ENetPeer* p);
        
        std::string GetData(std::string s); // returns string of data 
        std::set<std::string> GetOptions(); 

        void UserPass(std::string usr, std::string pass); //! For MP_LOGIN_REQ
        void Select(char s);

    private:
        enum MURK_PACKET_TYPES type;

        std::string str;
        std::map<std::string, std::string> data;
        std::set<std::string> p_options;

        ENetPeer* peer;
        
        //! Must be done first, called from constructor
        void SetType(enum MURK_PACKET_TYPES e);
        
};


}

#endif
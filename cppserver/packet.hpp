//packet.hpp

#include <string>
#include <map>

extern "C" {
  #include "murk.h"
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
        
        int Validate();     // Ensures str is a valid json blob 
        void ParseData();   // Stores payload in data map 
        
        std::string GetString();        // returns the json blob 
        void SetString(const char* s);  // sets the json blob 
        ENetPeer* GetPeer();
        void SetPeer(ENetPeer* p);
        
        std::string GetData(std::string s); // returns string of data 

        void UserPass(std::string usr, std::string pass); //! For MP_LOGIN_REQ

    private:
        enum MURK_PACKET_TYPES type;
        std::string str;
        std::map<std::string, std::string> data;
        ENetPeer* peer;

        //! Must be done first, called from constructor
        void SetType(enum MURK_PACKET_TYPES e);
        
};


}

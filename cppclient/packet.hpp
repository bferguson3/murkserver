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
        
        int Validate();
        void ParseData();
        
        std::string GetString();
        void SetString(const char* s);

        std::string GetData(std::string s);

        void UserPass(std::string usr, std::string pass); //! For MP_LOGIN_REQ

    private:
        enum MURK_PACKET_TYPES type;
        std::string str;
        std::map<std::string, std::string> data;

        //! Must be done first
        void SetType(enum MURK_PACKET_TYPES e);
        
};


}

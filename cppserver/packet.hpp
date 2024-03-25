//packet.hpp

#include <string>
extern "C" {
  #include "murk.h"
}

namespace Murk
{

class Packet 
{

    public:
        Packet();
        Packet(enum MURK_PACKET_TYPES e);
        
        int Validate();
        
        std::string GetString();
        void SetString(const char* s);

        void UserPass(std::string usr, std::string pass); //! For MP_LOGIN_REQ

    private:
        enum MURK_PACKET_TYPES type;
        std::string str;

        //! Must be done first
        void SetType(enum MURK_PACKET_TYPES e);
        
};


}

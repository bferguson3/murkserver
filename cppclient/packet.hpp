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
        Packet(enum MURK_PACKET_TYPES e);
        
        //! For MP_LOGIN_REQ
        void UserPass(std::string usr, std::string pass);

        std::string GetString();

    private:
        enum MURK_PACKET_TYPES type;
        std::string str;

        //! Must be done first
        void SetType(enum MURK_PACKET_TYPES e);
        
};


}

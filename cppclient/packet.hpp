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
        void SetType(enum MURK_PACKET_TYPES e);
        void AddUserPass(std::string usr, std::string pass);
        void Finalize();
        std::string GetString();

    private:
        enum MURK_PACKET_TYPES type;
        std::string str;

};


}

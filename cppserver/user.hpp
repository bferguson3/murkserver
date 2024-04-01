//user.hpp

extern "C" {
  #include "../murk.h"
}
#include <cstring>
#include <string>
#include "../cppclient/screen.hpp"

namespace Murk { 

class User 
{
    public:

        void  SetID(char* id);
        char* GetID();

        std::string GetLastPacket();
        void        SetLastPacket(std::string p);


    private:
        char        id[16]; // assigned guid 

        Screen*     currentScreen;
        PlayerState state;
        std::string last_packet;
        
        char        bytes[256];
};

}
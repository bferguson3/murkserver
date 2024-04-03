//user.hpp


#ifndef __USER_HPP__
#define __USER_HPP__

extern "C" {
  #include "../murk.h"
}
#include <cstring>
#include <string>
#include "item.hpp"

namespace Murk { 

class User 
{
    public:

        void      SetID(char* id);
        char*     GetID();

        std::string GetLastPacket();
        void        SetLastPacket(std::string p);

        void        UpdateStats();


    private:
        char        id[16]; // assigned guid 

        void*     currentScreen;
        PlayerState state;
        std::string last_packet;
        
        // Define custom character data 
#include "character_data.h"
};


}

#endif 
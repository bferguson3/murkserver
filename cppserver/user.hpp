//user.hpp


#ifndef __USER_HPP__
#define __USER_HPP__

extern "C" {
  #include "../murk.h"
}
#include <cstring>
#include <string>
#include <set>
#include "item.hpp"
#include "object.hpp"
//#include "screen.hpp"

namespace Murk { 

class User : public Object 
{
    public:
        void      SetID(char* id);
        char*     GetID();

        std::string GetLastPacket();
        void        SetLastPacket(std::string p);
        
        void        Equip(Item i);
        void        UpdateStats();

        void        SetScreen(void* s);
        


    private:
        char        id[16]; // assigned guid 

        PlayerState state;
        std::string last_packet;
        void* currentScreen;
        
        //std::string display_name;
        // Define custom character data 
#include "character_data.h"
};


}

#endif 
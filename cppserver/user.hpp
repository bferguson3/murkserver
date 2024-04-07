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
//#include "screen.hpp"

namespace Murk { 

class User 
{
    public:
        void      SetID(char* id);
        char*     GetID();

        std::string GetLastPacket();
        void        SetLastPacket(std::string p);
        
        void        Equip(Item i);
        void        UpdateStats();

        void SetScreen(void* s);
        


    private:
        char        id[16]; // assigned guid 

        PlayerState state;
        std::string last_packet;
        void* currentScreen;
        
        std::string display_name;
        // Define custom character data 

             int   level;
            long   exp;

             int   STR;
             int   DEX;
             int   WIS;

std::vector<Item>  inventory; // pointers to itemdb as items are static, count is held here
std::vector<Item*> equipment;  // points to a* in inventory<a,b>

};


}

#endif 
//user.hpp


#ifndef __USER_HPP__
#define __USER_HPP__

extern "C"
{
  #include <enet/enet.h>
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
        User();

        void      SetID(char* id);
        char*     GetID();
        
        void      PickUp(Item i, bool stackable, int ct);

        std::string GetLastPacket();
        void        SetLastPacket(std::string p);
        
        void        Equip(Item i);
        void        UpdateStats();

        void        SetScreen(void* s); // move immediately to...
        void*      GetScreen();

        void SetPeer(ENetPeer* p);
        ENetPeer* GetPeer();
      

    private:
        char        id[17]; // assigned guid 

        PlayerState state;
        std::string last_packet;
        void* currentScreen;
        ENetPeer* mypeer;

        
        
        //std::string display_name;
        // Define custom character data 
#include "character_data.h"
};


}

#endif 
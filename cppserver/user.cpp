//user.cpp

#include "user.hpp"
#include "server.hpp"
extern class Murk::Server server;

namespace Murk 
{

  //  extern Server server;

    void User::PickUp(Item i, bool stackable, int ct) { 

        Item _i;
        _i = i;
        _i.SetStackable(stackable);
        _i.SetStackCount(ct);
        inventory.push_back(_i);

        server.SendLocalMessage(GetScreen(), "{0} picks up the {1}.", display_name, _i.display_name);
    }

    void User::SetID(char* _id)
    {
        memcpy(id, _id, 16);
    }
    char* User::GetID()
    {
        return &id[0];
    }

    void        User::SetScreen(void* s) { currentScreen = s; }
    void*     User::GetScreen() { return (void*)currentScreen; }
    std::string     User::GetLastPacket() { return last_packet; }
    void            User::SetLastPacket(std::string p) { last_packet = p; }

}

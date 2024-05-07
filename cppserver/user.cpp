//user.cpp

#include "user.hpp"
#include "server.hpp"
extern class Murk::Server server;

namespace Murk 
{


  //  extern Server server;
    User::User()
    {
        currentScreen = 0;
    }

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
        id[16] = 0;
    }
    char* User::GetID()
    {
        return &id[0];
    }

    void  User::SetScreen(void* s) 
    { 
        Screen* _s = (Screen*)currentScreen;
        // leave current screen 
        if(currentScreen != 0){
            printf("current screen not 0, so.. \n");
            _s->ExitUser(GetID()); // Delete user from screen by ref
        }
        // move to next 
        _s = (Screen*)s;
        printf("%s\n", _s->GetDescription().c_str());
        _s->EnterUser(GetID());

        currentScreen = s; 
    
    }
    void*     User::GetScreen() { return (void*)currentScreen; }
    std::string     User::GetLastPacket() { return last_packet; }
    void            User::SetLastPacket(std::string p) { last_packet = p; }
    void User::SetPeer(ENetPeer* p) { mypeer = p;}
    ENetPeer* User::GetPeer() { return mypeer; }

    

  
}

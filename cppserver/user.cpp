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

    void User::PickUp(Item i, int ct) { 
        
        // TODO : multiple checks here 
        Item _i;
        _i = i;
        
        _i.SetStackCount(ct);
        inventory.push_back(_i);

        // finally, send message if all is clear 
        std::string s = display_name + " picks up the " + _i.display_name + ".";
        server.SendLocalMessage(GetScreen(), s);
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
            //printf("[DEBUG] Current screen not null. Removing from <vec>\n");
            _s->ExitUser(GetID()); // Delete user from screen by ref
        }
        // move to next 
        _s = (Screen*)s;
        _s->EnterUser(GetID());

        //printf("[DEBUG] Room short desc: %s\n", _s->GetName().c_str());
        //printf("[DEBUG] Room description: %s\n", _s->GetDescription().c_str());
        // print exits? 

        currentScreen = s; 

        // send a packet with all of the room info. 
        Murk::Packet _p = Packet(MP_ROOM_INFO); 
        _p.AddScreen(_s);
        
        if(!!_p.Validate())
        {
            printf("ERROR: Packet invalid! Not sent.\n");
            return;
        }
        
        server.SendPacket(GetPeer(), _p);
    
    }
    void*     User::GetScreen() { return (void*)currentScreen; }
    std::string     User::GetLastPacket() { return last_packet; }
    void            User::SetLastPacket(std::string p) { last_packet = p; }
    void User::SetPeer(ENetPeer* p) { mypeer = p;}
    ENetPeer* User::GetPeer() { return mypeer; }

    

  
}

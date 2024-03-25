
#include "packet.hpp"

namespace Murk { 

Packet::Packet()
{
    str = "{\n \"type\":\"";
    
}

void Packet::SetType(enum MURK_PACKET_TYPES t)
{
    type = t;
    switch(t){
        case MP_LOGIN_REQ:
            str += "LOGIN_REQ\",\n ";
            break;
        case MP_MENUSEL:
            break;
        case MP_PCOMMAND:
            break;
    }
    
}

void Packet::AddUserPass(std::string usr, std::string pass)
{
    if(type != MP_LOGIN_REQ) {
        printf("Fatal error: Mismatch packet type in adduserpass()\n");
        exit(1);
    }

    str += "\"user\":\"";
    str += usr;
    str += "\",\n \"";
    str += "pass\":\"";
    str += pass;
    str += "\"";
}

void Packet::Finalize()
{
    str += "\n}";
}

std::string Packet::GetString() { return str; }

}

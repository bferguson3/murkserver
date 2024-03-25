
#include "packet.hpp"

namespace Murk { 

//! Constructing a packet requires the first json field 
//!  to be "type", so we fill that out here. 
Packet::Packet(enum MURK_PACKET_TYPES t)
{
    str = "{\n \"type\":\"";
    type = t;
    switch(t){
        case MP_LOGIN_REQ:
            str += "LOGIN_REQ\",\n ";
            break;
        case MP_MENUSEL:
            str += "MENUSEL\",\n ";
            break;
        case MP_PCOMMAND:
            str += "PCOMMAND\",\n ";
            break;
    }
}

//! Only used for LOGIN_REQ
void Packet::UserPass(std::string usr, std::string pass)
{
    if(type != MP_LOGIN_REQ) {
        printf("Fatal error: Mismatch packet type in userpass()\n");
        exit(1);
    }

    str += "\"user\":\"";
    str += usr;
    str += "\",\n \"";
    str += "pass\":\"";
    str += pass;
    str += "\"\n}";

}

std::string Packet::GetString() { return str; }

}

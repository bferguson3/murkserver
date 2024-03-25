
#include "packet.hpp"
#include "../res/json.h"

namespace Murk { 

Packet::Packet()
{
    str = "";
    type = MP_NOPACKET;
}

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

int Packet::Validate()
{
    // parse json object
    struct json_value_s *j = json_parse(GetString().c_str(), GetString().length()); 
    if(j == 0) {
        printf("Fatal error: Error parsing JSON: %s\n", GetString().c_str());
        return -1;
    }
    return 0;
}


std::string Packet::GetString() { return str; }
void Packet::SetString(const char* s) { str = s; }

}


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
        case MP_LOGIN_WELCOME:
            str += "LOGIN_WELCOME\",\n ";
    }
}

Packet::Packet(const char* s)
{
    str = s;
    ParseData();
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


void Packet::ParseData()
{
    JSONVal j = json_parse(GetString().c_str(), GetString().length());

    if(j == 0) {
        printf("Error: Packet could not be parsed: %s", GetString().c_str());
        return;
    }

    JSONObject object = (JSONObject)j->payload;
    JSONElement p_type = object->start;
    JSONString p_type_str = p_type->name;

    std::string _s = p_type_str->string;
    std::string _q = "type";
    if(_s != _q) return; // Invalid packet - just ignore it 

    // get the packet type
    JSONVal p_type_val = p_type->value;
    JSONString p_type_valstr = (JSONString)p_type_val->payload;

    // Debug print the packet type
    printf("[Debug] packet type: %s\n", p_type_valstr->string);
    //std::string type = p_type_valstr->string; // store it in the class 
    data[_q] = p_type_valstr->string;

    // assign last json element
    JSONElement last_j = p_type;
    // continue while there are still elements in the blob
    while (last_j->next != json_null)
    {
        JSONElement next_j = last_j->next; // get next element

        JSONString next_name = next_j->name;
        JSONVal next_val = next_j->value;
        JSONString next_str = (JSONString)next_val->payload;

        data[(std::string)next_name->string] = (std::string)next_str->string; // e.g data["user"]="name"
        //printf("%s , %s\n", next_name->string, data[next_name->string].c_str());
        last_j = next_j;
    }

    free(j);
    //printf("[Debug] JSON parsed successfully.\n");
}


std::string Packet::GetData(std::string s)
{
    return data[s];
}


ENetPeer* Packet::GetPeer() { return peer; }
void Packet::SetPeer(ENetPeer* p) { peer = p; printf("set to %p\n", peer); }

std::string Packet::GetString() { return str; }
void Packet::SetString(const char* s) { str = s; }

}

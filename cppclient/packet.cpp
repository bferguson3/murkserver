
#include "packet.hpp"
#include "../res/json.h"
#include "../cppserver/screen.hpp"

namespace Murk { 

Packet::Packet()
{
    str = "";
    type = MP_NOPACKET;
}

/*
{
"type":"ROOM_INFO",
"name":$shortdesc,
"desc":$longdesc,
"exits":int,
"users":[],
"items":[]
}
*/

void Packet::AddScreen(Screen* s)
{
    str += "\"name\":\"" + s->GetName() + "\",\n \"desc\":\"" + s->GetDescription() + "\",\n"; 
    str += " \"exits\":\"" + std::to_string(s->ExitsToInt()) + "\"\n}";
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
        case MP_NOPACKET:
            break;
        case MP_MENUSEL:
            str += "MENUSEL\",\n ";
            break;
        case MP_PCOMMAND:
            str += "PCOMMAND\",\n ";
            break;
        case MP_LOGIN_WELCOME:
            str += "LOGIN_WELCOME\",\n ";
            break;
        case MP_MESSAGE_GEN:
            str += "MESSAGE_GEN\",\n ";
            break;
        case MP_MESSAGE_SCREEN:
            str += "MESSAGE_SCREEN\",\n ";
            break;
        case MP_ROOM_INFO:
            str += "ROOM_INFO\",\n ";
            break;
    }
}

//
// These two are used together 
void Packet::SetMessage(std::string msg)
{
    str += "\"text\":\"";
    str += msg;
    str += "\",\n";
}
void Packet::SetScreen(int id)
{
    str += "\"screen\":\"";
    str += std::to_string(id);
    str += "\"\n}";
}
///
//

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

void Packet::AddCommand(std::string s) // USE x on y 
{
    str += " \"cmd\":\"" + s + "\",\n";
}
void Packet::AddTarget(std::string s) // use x on _Y_ 
{
    str += " \"tgt\":\"" + s + "\",\n";
}
void Packet::AddSubject(std::string s) // use _X_ on y 
{
    str += " \"s\":\"" + s + "\"\n}\x00\x00";
}

void Packet::Select(char s)
{
    if(type != MP_MENUSEL) {
        printf("Fatal error: Mismatch packet type in Select()\n");
        exit(1);
    }

    str += "\"select\":\"";
    str += s;
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
    data[_q] = p_type_valstr->string;

    // assign last json element
    JSONElement last_j = p_type;
    // continue while there are still elements in the blob
    while (last_j->next != json_null)
    {
        JSONElement next_j = last_j->next; // get next element

        JSONString next_name = next_j->name;
        JSONVal next_val = next_j->value;
        struct json_array_s * array;
        array = json_value_as_array(next_val);
        if(array == json_null) // not an array 
        {
            JSONString next_str = (JSONString)next_val->payload;
            data[(std::string)next_name->string] = (std::string)next_str->string; // e.g data["user"]="name"
            
        }
        else { // is an array 
            // get list of menu options
            int arlen = array->length;  // and length
            
            struct json_array_element_s *this_ele = array->start; // get first element
            JSONString ele_str = json_value_as_string(this_ele->value);
            
            p_options.insert(ele_str->string); // push to vec
            for (int c = 1; c < arlen; c++)
            {
                this_ele = this_ele->next;
                ele_str = json_value_as_string(this_ele->value);
                p_options.insert(ele_str->string);
            }  // and the remaning options
        }
        last_j = next_j;
    }

    free(j);
    //printf("[Debug] JSON parsed successfully.\n");
}

std::set<std::string>   Packet::GetOptions() { return p_options; }
std::string             Packet::GetData(std::string s) {    return data[s];   }
ENetPeer*               Packet::GetPeer() { return peer; }
void                    Packet::SetPeer(ENetPeer* p) { peer = p; }
std::string             Packet::GetString() { return str; }
void                    Packet::SetString(const char* s) { str = s; }

}

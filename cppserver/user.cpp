//user.cpp

#include "user.hpp"

namespace Murk 
{

    void User::SetID(char* _id)
    {
        memcpy(id, _id, 16);
    }
    char* User::GetID()
    {
        return &id[0];
    }

    std::string     User::GetLastPacket() { return last_packet; }
    void            User::SetLastPacket(std::string p) { last_packet = p; }

}

//user.cpp

#include "user.hpp"
#include <cstring>

namespace Murk 
{

    void User::SetID(char* _id)
    {
        memcpy(id, _id, 16);
    }

}

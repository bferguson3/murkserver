// mob.hpp
// Any non-player mobile. 
#ifndef __MOB_HPP__
#define __MOB_HPP__


#include <map>
#include "command.hpp"

namespace Murk { 

class Mob { 
    public:
        Mob();

    private:

        std::string name;
        std::string desc;

#include "mob_data.h"
};
}

#endif 
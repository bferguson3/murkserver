// mob.hpp
// Any non-player mobile. 
#ifndef __MOB_HPP__
#define __MOB_HPP__

#include <map>
#include "command.hpp"
//#include "item.hpp"

namespace Murk { 

class Mob { 
    public:
        Mob();

    private:

        std::string display_name;
        
};


}

#endif 
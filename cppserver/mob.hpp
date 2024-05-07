// mob.hpp
// Any non-player mobile. 
#ifndef __MOB_HPP__
#define __MOB_HPP__

#include <map>
#include "command.hpp"
#include "object.hpp"
#include "item.hpp"

namespace Murk { 

class Mob : public Object { 
    
    public:
        //std::string display_name; inherited 
        Mob();

        void PickUp(Item i);

    private:

#include "mob_data.h"
};


}

#endif 
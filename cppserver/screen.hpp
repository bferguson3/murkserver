// screen.hpp 

#ifndef __SCREEN_HPP__
#define __SCREEN_HPP__

extern "C" {
    #include "../murk.h"
}
#include <string>
#include "user.hpp"
#include "item.hpp"
#include "mob.hpp"

namespace Murk { 


class Screen 
{
    public:
        Screen();
        Screen(enum ScreenType t, Exits e, std::string desc);

    private:    
        int id;

        enum ScreenType type;

        Exits exits;
        std::string shortdesc;
        std::string description;

        // The vectors of class objects are only filled as they are needed. 
        std::vector<Item>   localItems;
        std::vector<Mob>    localMobs;
        std::vector<Murk::User>   localUsers; // e.g. full data here is not sent 
};

}

#endif 

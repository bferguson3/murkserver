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
        Screen(enum ScreenType t, Exits e, std::string desc, std::string longdesc);

        static int GetScreenCount();

        std::string GetDescription();

    private:    
        int id;

        enum ScreenType type;

        Exits exits;
        std::string shortdesc;
        std::string description;

        // The vectors of class objects are only filled as they are needed. 
        std::vector<Item*>   localItems; // Items cannot be maipulated as they are pointers
        std::vector<Mob>     localMobs;      // Mobs must be instantiated 
        std::vector<User*>   localUsers; // e.g. full data here is not sent 
            // On client side, localUsers are allocated/instantiated upon 
            //  entering rooms. On the server they track users as they move.
};

extern const Screen mainmenu;

}

#endif 

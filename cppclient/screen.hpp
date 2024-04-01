// screen.hpp 
#ifndef __SCREEN_HPP__
#define __SCREEN_HPP__

extern "C" {
    #include "../murk.h"
}
#include <string>

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
        std::string description;
};

}

#endif 

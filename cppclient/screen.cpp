// screen.cpp 

#include "screen.hpp"
#include <string>

extern "C"
{
    #include "../murk.h"
}

namespace Murk { 

    static int SCREEN_COUNT = 0;

    Screen::Screen()
    {
        type = SCR_LOGIN;
        id = SCREEN_COUNT;
        Exits _e = { no_exits };
        exits = _e;
        description = "None";

        SCREEN_COUNT++;
    }

    Screen::Screen(enum ScreenType t, Exits e, std::string desc)
    {
        type = t;
        exits = e;
        description = desc;
        id = SCREEN_COUNT;

        SCREEN_COUNT++;
    }

}
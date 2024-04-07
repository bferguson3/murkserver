// screen.cpp 

#include "screen.hpp"


namespace Murk { 

    static int SCREEN_COUNT = 0;

    Screen::Screen()
    {
        type = SCR_LOGIN;
        id = SCREEN_COUNT;
        Exits _e = { no_exits };
        exits = _e;
        shortdesc = "[None]";
        description = "There is no description.";

        SCREEN_COUNT++;
    }

    int Screen::GetScreenCount() { return SCREEN_COUNT; }

    std::string Screen::GetDescription() { return description; }

    Screen::Screen(enum ScreenType t, Exits e, std::string desc, std::string longdesc)
    {
        type = t;
        exits = e;
        shortdesc = desc;
        description = longdesc;
        id = SCREEN_COUNT;

        SCREEN_COUNT++;
    }

}
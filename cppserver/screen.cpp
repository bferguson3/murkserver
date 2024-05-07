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

    void Screen::EnterUser(const char* u)
    {
        localUsers.push_back(u);
        printf("added user %s\n", u);
    }

    void Screen::ExitUser(const char* u){
        
    }
    
    std::string Screen::GetName() { return shortdesc; }
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

    int Screen::GetLocalUserCt() { 
        return localUsers.size();
    }

    std::string Screen::GetUserByIndex(int i)
    {
        printf("returning %s from screen index query\n", localUsers[i].c_str());
        return localUsers[i];
    }

    int Screen::GetID() { return id; }

}
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

    void Screen::Execute(User u, int i)
    {
        void (*f)(User);
        f = (menuFunctions[i]);
        f(u);
    }

    void Screen::EnterUser(const char* u)
    {
        localUsers.push_back(u);
        printf("[DEBUG] added user %s to room %d\n", u, GetID());
    }

    void Screen::ExitUser(const char* u){
        // TODO 
        std::string n = u;
        std::vector<std::string>::iterator it;
        for(it = localUsers.begin(); it != localUsers.end(); it++)
        {
            if(n.compare(0, 16, *it) == 0){
                // match 
                //printf("adfasfed\n");
                localUsers.erase(it);
                break;
                //localUsers.erase(localUsers.begin() + y);
            }
        }
        
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

    void Screen::SetMenu(std::vector<void(*)(User)> opts)
    {
        menuFunctions = opts;
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
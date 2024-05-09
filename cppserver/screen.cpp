// screen.cpp 

#include "screen.hpp"
#include "../cppclient/packet.hpp"

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

    Screen::Screen(enum ScreenType t, Exits e, std::string desc, std::string longdesc)
    {
        type = t;
        exits = e;
        shortdesc = desc;
        description = longdesc;
        id = SCREEN_COUNT;

        SCREEN_COUNT++;
    }


    void Screen::Execute(User* u, int i)
    {
        void (*f)(User*);
        f = (menuFunctions[i]);
        f(u);
    }

    void Screen::EnterUser(const char* u)
    {
        localUsers.push_back(u);
        
        printf("[DEBUG] added user %s to room %d\n", u, GetID());
        printf("[DEBUG] User count: %lu\n", localUsers.size());

    }

    void Screen::ExitUser(const char* u){
        std::string n = u;
        std::vector<std::string>::iterator it;
        for(it = localUsers.begin(); it != localUsers.end(); it++)
        {
            if(n.compare(0, 16, *it) == 0){
                // match 
                localUsers.erase(it);
                break;
            }
        }
        printf("[DEBUG] User count: %lu\n", localUsers.size());
    }

    int Screen::ExitsToInt()
    {
        int e = 0;
        if(exits.N) e |= 1;
        if(exits.S) e |= (1 << 1);
        if(exits.E) e |= (1 << 2);
        if(exits.W) e |= (1 << 3);
        if(exits.Up) e |= (1 << 4);
        if(exits.Down) e |= (1 << 5);
        return e;
    }
    
    std::string Screen::GetName() { return shortdesc; }
    int Screen::GetScreenCount() { return SCREEN_COUNT; }
    std::string Screen::GetDescription() { return description; }

    void Screen::SetMenu(std::vector<void(*)(User*)> opts)
    {
        menuFunctions = opts;
    }


    int Screen::GetLocalUserCt() { 
        return localUsers.size();
    }

    std::string Screen::GetUserByIndex(int i)
    {
        return localUsers[i];
    }

    int Screen::GetID() { return id; }

}
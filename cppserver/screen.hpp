// screen.hpp 

#ifndef __SCREEN_HPP__
#define __SCREEN_HPP__

extern "C" {
    #include "../murk.h"
}
#include <string>
#include <vector>
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
        std::string GetName();
        int GetID();
        int GetLocalUserCt();
        std::string GetUserByIndex(int i);

        void EnterUser(const char* guid);
        void ExitUser(const char* guid); // match to User*

        void Execute(User u, int i);

        void SetMenu(std::vector<void(*)(User)> opts);
        

    private:    
        int id;

        enum ScreenType type;

        Exits exits;
        std::string shortdesc;
        std::string description;

        // The vectors of class objects are only filled as they are needed. 
        std::vector<Item*>   localItems;    // Items cannot be maipulated as they are pointers
        std::vector<Mob>     localMobs;     // Mobs must be instantiated 
        std::vector<std::string>   localUsers;    // by GUID only, for refereence 

        std::vector<void(*)(User)>   menuFunctions;
};

//extern const Screen mainmenu;

}

#endif 

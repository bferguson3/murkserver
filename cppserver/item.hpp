#ifndef __ITEM_HPP__
#define __ITEM_HPP__

#include <string> 
//#include "user.hpp"

namespace Murk { 


class Item {

    public:
        //void setCallback(void(*f)(User*));
        void (*use)(int*);
        //void            (* use)(Murk::User*);

    private:
        std::string     name;
        std::string     desc;
        //enum ItemType   type;
        bool            stackable;
        int             stack_ct;

        uint64_t        flags; // 64 flags to use, check murk.h
        

        void    drop();     // place in the screen 
        void    destroy();  // completely remove from game 
};

}

#endif 

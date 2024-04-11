#ifndef __ITEM_HPP__
#define __ITEM_HPP__

#include <string> 
#include "object.hpp"
//#include "user.hpp"

namespace Murk { 


class Item : public Object {

    public:
    //std::string     display_name; // inherited
        void (*use)(int*);

    private:
        std::string     desc;
        
        bool            stackable;
        int             stack_ct;

        uint64_t        flags; // 64 flags to use, check murk.h
        

        void    drop();     // place in the screen 
        void    destroy();  // completely remove from game 
};

}

#endif 

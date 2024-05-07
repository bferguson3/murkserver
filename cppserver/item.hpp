#ifndef __ITEM_HPP__
#define __ITEM_HPP__

#include <string> 
#include "object.hpp"
//#include "user.hpp"

namespace Murk { 


class Item : public Object {

    public:
    //std::string     display_name; // inherited
        Item();
        void (*use)(int*);

        void SetStackable(bool tf);
        void SetStackCount(int i);
        void SetFlags(uint64_t i);

    private:
        std::string     desc;
        
        bool            stackable;
        int             stack_ct;

        uint64_t        flags; // 64 flags to use, check murk.h
        

        void    drop();     // place in the screen 
        void    destroy();  // completely remove from game 
};

/*
#define ITEMFLAG_EQUIPPABLE_FIGHTER (1 << 0)
#define ITEMFLAG_EQUIPPABLE_RANGER  (1 << 1)
#define ITEMFLAG_EQUIPPABLE_MAGE    (1 << 2)
#define ITEMFLAG_EQUIP_BODY         (1 << 3)
#define ITEMFLAG_EQUIP_ARM          (1 << 4)
#define ITEMFLAG_EQUIP_WEAPON       (1 << 5)
#define ITEMFLAG_EQUIP_CURSE        (1 << 6)
#define ITEMFLAG_JUNK               (1 << 7)
#define ITEMFLAG_MAGICAL            (1 << 8)
#define ITEMFLAG_USE_CONSUME        (1 << 9)
#define ITEMFLAG_USEABLE_BATTLE     (1 << 10)
#define ITEMFLAG_USEABLE_MAP        (1 << 11)
*/
}

#endif 

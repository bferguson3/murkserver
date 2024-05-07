//item.cpp
#include "item.hpp"
#include "murk.h"

namespace Murk { 

    Item::Item()
    {
        desc = "";
        stackable = false;
        stack_ct = true;
        //
        flags = 0;
        //use = &drop();       
    }

    void Item::SetStackable(bool tf){stackable = tf;};
    void Item::SetStackCount(int i){stack_ct = i;};
    void Item::SetFlags(uint64_t f) { flags = f; }

};
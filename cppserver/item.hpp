#ifndef __ITEM_HPP__
#define __ITEM_HPP__

namespace Murk { 

enum ItemType { 
    IT_UNUSABLE   = 0,
    IT_ONLYMAP    = 1,
    IT_ONLYBATTLE = 2,
    IT_EVERYWHERE = 3,
    IT_EQUIPMENT  = 4
};

class Item {

    private:
        std::string     name;
        std::string     desc;
        enum ItemType   type;
        uint32_t        flags;
        void          (* use)();

        void    drop();
        void    destroy();
        void    equip();
};

}

#endif 
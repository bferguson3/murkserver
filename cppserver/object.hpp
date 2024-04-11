#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

enum ObjectType { 
    OBJ_NONE = 0,
    OBJ_PLAYER = 1,
    OBJ_MOB = 2,
    OBJ_ITEM = 3
};

namespace Murk { 
    class Object { 

    public: 
        std::string      display_name;
        //void*            obj;
        //Murk::ObjectType type;

};
}

#endif 
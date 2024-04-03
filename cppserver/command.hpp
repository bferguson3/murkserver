#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__


namespace Murk { 
// mob cmd 
// 20 flee 
// 40 fireball 
// [100 attack] always < 

class Command { 
    public: 
        Command();

    private:
        std::string name;
        std::string desc;
        void(*activate)();
};

}

#endif 
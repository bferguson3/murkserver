//user.hpp
namespace Murk { 

class User 
{
    public:
        char id[16]; // assigned guid 

        Screen* currentScreen;
        PlayerState state;
        
        char bytes[256];
};

}
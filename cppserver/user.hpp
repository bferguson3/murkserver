//user.hpp

extern "C" {
  #include "../murk.h"
}

namespace Murk { 

class User 
{
    public:

        void SetID(char* id);

    private:
        char id[16]; // assigned guid 

        Screen* currentScreen;
        PlayerState state;
        
        char bytes[256];
};

}

//NAME,FLAGS,CALLBACKNAME 
//"Potion",ITEMFLAG_USE_CONSUME|ITEMFLAG_USEABLE_BATTLE|ITEMFLAG_USEABLE_MAP,use_potion
#include <stdio.h>

namespace Murk { 

void use_potion(int* target)
{ 
    printf("test");
}

}
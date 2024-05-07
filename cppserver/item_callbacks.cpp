
//NAME,FLAGS,CALLBACKNAME 
//"Potion",ITEMFLAG_USE_CONSUME|ITEMFLAG_USEABLE_BATTLE|ITEMFLAG_USEABLE_MAP,use_potion
#include <stdio.h>
#include "../murk.h"
#include "item.hpp"

namespace Murk { 

void use_potion(int* target);

void use_potion(int* target)
{ 
    printf("TEST USE POTION\n");
}

}
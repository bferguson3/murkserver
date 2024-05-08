#ifndef __MAINMENU_HPP__
#define __MAINMENU_HPP__

#include "server.hpp"

namespace Murk { 


const Exits noexit = { no_exits };

void new_game(User* u);
void quit_game(User* u);
void InitMainMenu();

}

#endif 
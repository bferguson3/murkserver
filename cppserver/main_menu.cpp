#include "main_menu.hpp"

namespace Murk { 

void new_game(User u)
{
	printf("new game run by %s\n", u.display_name.c_str());
}

void quit_game(User u)
{
	printf("quit game run\n");
}



}
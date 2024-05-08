#include "main_menu.hpp"

#define MURK_STARTSCREEN 1 

extern Murk::Server server;
extern std::vector<void(*)(Murk::User*)> mm_opt;
extern Murk::Screen mainmenu;

namespace Murk { 

void new_game(User* u)
{
	printf("new game run by %s\n", u->display_name.c_str());
	// TODO : 
	//   1 Send make new char flow 
	//	 2 Make new char entry in SQL db
	//   3 Send new game flow to user 
	//   4 Set user screen 
	u->SetScreen((void*)&server.screensList[MURK_STARTSCREEN]); //   
}

void quit_game(User* u)
{
	printf("quit game run\n");
}


// BEYOND HERE WILL BE GENERATED CODE AUTOMATICALLY . 

void InitMainMenu()
{
	// init somehow?
	mm_opt.push_back((void(*)(Murk::User*))nullptr); // index 0 
	mm_opt.push_back((void(*)(Murk::User*))&Murk::new_game);
	mm_opt.push_back((void(*)(Murk::User*))&Murk::quit_game);
	mainmenu.SetMenu(mm_opt);
	server.AddScreen(mainmenu);


}


}
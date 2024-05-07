//main.cpp

#include "server.hpp"
#include "screen.hpp"

#include "main_menu.hpp"


void InitMainMenu();

// const scr definitions - loaded from data files 
// id, type, exits, shortdesc, desc, items/mobs
//   if items/mobs is not empty then it will spawn once every SPAWNTICKRATE 
const Exits NSEW = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };
// callbacks
namespace Murk { 

	std::vector<void(*)(User)> mm_opt;
	Murk::Screen mainmenu(SCR_LOGIN, noexit, "Main Menu", "Main menu example for MURK");

	//const 
	const Screen test1(SCR_NORMAL_AREA, NSEW, "Grassy Hill", "There is nothing here yet!");
	void use_potion(int* target);
}


class Murk::Server server;


int main()
{
	server.Init();
	
	// Set up all screens : 
	InitMainMenu(); //adds screen #0 
	server.AddScreen(Murk::test1);
		
	printf("[DEBUG] screen count %d\n", Murk::Screen::GetScreenCount());

	
	// Set up inventory database: 
	Murk::Item potion; // allocate
	potion.use = &Murk::use_potion; // set meta
	potion.display_name = "Potion";
	potion.SetFlags(ITEMFLAG_USE_CONSUME|ITEMFLAG_USEABLE_BATTLE|ITEMFLAG_USEABLE_MAP);

		// TEST AUTO USER 
	
	Murk::User _u;	// allocate 
	_u.SetID("g00b3rg00b3r");
	_u.SetScreen(&server.screensList[0]); // meta 
	_u.display_name = "Test!!!User";
	_u.PickUp(potion, true, 1); // action 
	potion.use((int*)(&_u));
	_u.SetScreen(&server.screensList[1]);
	

	ENetEvent event;
	while (1)
	{
		while (enet_host_service(server.server, &event, 1) > 0)  // is there an event?
		{
			server.ProcessEvent(event);
		}
		// Other processing (input etc.):

	}

	// De-init server
	server.DeInit();

	return 0;

}


void InitMainMenu()
{
	// init somehow?
	Murk::mm_opt.push_back((void(*)(Murk::User))&Murk::new_game);
	Murk::mm_opt.push_back((void(*)(Murk::User))&Murk::quit_game);
	Murk::mainmenu.SetMenu(Murk::mm_opt);
	server.AddScreen(Murk::mainmenu);


}
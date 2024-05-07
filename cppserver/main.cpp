//main.cpp

#include "server.hpp"
#include "screen.hpp"


// const scr definitions - loaded from data files 
// id, type, exits, shortdesc, desc, items/mobs
//   if items/mobs is not empty then it will spawn once every SPAWNTICKRATE 
const Exits noexit = { no_exits };
const Exits NSEW = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };
// callbacks
namespace Murk { 
	const Screen mainmenu(SCR_LOGIN, noexit, "Main Menu", "Main menu example for MURK");
	const Screen test1(SCR_NORMAL_AREA, NSEW, "Grassy Hill", "There is nothing here yet!");
 	void use_potion(int* target);
}

class Murk::Server server;

int main()
{
	server.Init();
	server.AddScreen(Murk::mainmenu);
	server.AddScreen(Murk::test1);

		// debug:
	//Murk::Screen scr;
	//scr = Murk::mainmenu;
	printf("scrtest%d\n", Murk::Screen::GetScreenCount());

	
	// Define new item:
	Murk::Item potion; // allocate
	potion.use = &Murk::use_potion; // set meta
	potion.display_name = "Potion";
	potion.SetFlags(ITEMFLAG_USE_CONSUME|ITEMFLAG_USEABLE_BATTLE|ITEMFLAG_USEABLE_MAP);

	Murk::User _u;	// allocate 
	_u.SetScreen((void*)&Murk::test1); // meta 
	_u.display_name = "Test User";

	_u.PickUp(potion, true, 1); // action 
	potion.use((int*)(&_u));
	


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



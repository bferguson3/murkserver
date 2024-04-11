//main.cpp

#include "server.hpp"
#include "screen.hpp"


// const scr definitions - loaded from data files 
// id, type, exits, shortdesc, desc, items/mobs
//   if items/mobs is not empty then it will spawn once every SPAWNTICKRATE 
const Exits noexit = { no_exits };
// callbacks
namespace Murk { 
	const Screen mainmenu(SCR_LOGIN, noexit, "Main Menu", "Main menu example for MURK");
 	void use_potion(int* target);
}


int main()
{
	class Murk::Server server;

		// debug:
	//Murk::Item potion;
	//potion.use = &Murk::use_potion;
	//Murk::User _u;
	//potion.use((int*)(&_u));
	//Murk::Screen scr;
	//scr = Murk::mainmenu;
	//printf("%d\n", Murk::Screen::GetScreenCount());

	server.Init();

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



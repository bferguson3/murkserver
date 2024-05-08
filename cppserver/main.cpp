//main.cpp

#include "server.hpp"
#include "screen.hpp"

#include "main_menu.hpp"


void Initialize_ItemDB();
extern void Murk::InitMainMenu();

// const scr definitions - loaded from data files 
// id, type, exits, shortdesc, desc, items/mobs
//   if items/mobs is not empty then it will spawn once every SPAWNTICKRATE 
const Exits NSEW = { 1, 1, 1, 1, 0, 0};//, 0, 0, 0, 0 };

const Exits noexit = { no_exits };

Murk::Server server;
std::vector<void(*)(Murk::User*)> mm_opt;
Murk::Screen mainmenu(SCR_LOGIN, noexit, "Main Menu", "Main menu example for MURK");
	
// callbacks
namespace Murk { 

	Item potion; // allocate
	
	//const 
	const Screen test1(SCR_NORMAL_AREA, NSEW, "Grassy Hill", 
"Thick sheaves of grass and dandelions dance at your knees. Hills of emerald\\n\
 and white roll off in all directions, though a small gathering of dots in\\n\
 the southeast tells you that a settlement is nearby.");
	void use_potion(int* target);
}


//class Murk::Server server;


int main()
{
	server.Init();
	
	// Set up all screens : 
	Murk::InitMainMenu(); //adds screen #0 
	server.AddScreen(Murk::test1);
		
	printf("[DEBUG] screen count %d\n", Murk::Screen::GetScreenCount());

	
	// Set up inventory database: 
	Initialize_ItemDB();


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

// DO NOT MODIFY 
void Initialize_ItemDB(){
    
	Murk::potion.use = &Murk::use_potion; // set meta
	Murk::potion.display_name = "Potion";
	Murk::potion.SetFlags(ITEMFLAG_USE_CONSUME|ITEMFLAG_USEABLE_BATTLE|ITEMFLAG_USEABLE_MAP);
}
	
//main.cpp

#include "server.hpp"

int main()
{
	class Murk::Server server;

	server.InitEnet();
	server.InitSQL();

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
	enet_host_destroy(server.server);
	atexit(enet_deinitialize);
	sqlite3_close(server.murk_userdb);

	return 0;

}

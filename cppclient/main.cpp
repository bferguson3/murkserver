//main.cpp
#include <enet/enet.h>
#include "client.hpp"
#include <cstdio>
#include <string.h>
#include "packet.hpp"

extern "C" {
  #include "../res/json.h"
}

extern int _getc();
void clear(void* dat, size_t ct);

int main()
{
    class Murk::Client client;

    client.SetNonblocking(); 
    client.InitEnet();

    // localhost target for testing
    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    client.Connect(&address);
    client.state = STATE_LOGGING_IN; 

    // Allocate buffer for kb input
    char input[256];
    clear(&input, 256);
    while (1) // Main loop 
    {
        ENetEvent event;
        while (enet_host_service(client.host, &event, 1) > 0)  // is there an event?
        {
            client.ProcessEvent(&event);
        }

        client.GameLoop();

        // Other processing (input etc:)
        client.ProcessInput(input);
        
    }

    client.Disconnect();// De-init 

    return 0;

}


void clear(void* dat, size_t ct)
{
    char* _v = (char*)dat;
    for(size_t i = 0; i < ct; i++) { _v[i] = '\00'; }
}



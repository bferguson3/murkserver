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
    ////// TESTING 
    class Murk::Packet pak(MP_LOGIN_REQ);
    pak.UserPass("ben", "1234");
    
    // parse json object
    struct json_value_s *j = json_parse(pak.GetString().c_str(), pak.GetString().length()); 
    if(j == 0) {
        printf("Fatal error: Error parsing JSON: %s\n", pak.GetString().c_str());
        exit(1);
    }
    ////////


    class Murk::Client client;

    client.SetNonblocking(); // not needed?

    client.InitEnet();

    // localhost target for testing
    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;
    
    client.Connect(&address);

    ENetEvent event;

    // Allocate buffer for kb input
    char input[256];
    clear(&input, 256);
    while (1) // Main loop 
    {
        while (enet_host_service(client.host, &event, 1) > 0)  // is there an event?
        {
            client.ProcessEvent(&event);
        }

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



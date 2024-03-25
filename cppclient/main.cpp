//main.cpp
#include <enet/enet.h>
#include "client.hpp"
#include <cstdio>
#include <string.h>
#include "packet.hpp"

extern "C" {
  #include "../res/json.h"
}

int _getc();
void clear(void* dat, size_t ct);


int main()
{
    ////// TESTING 
    class Murk::Packet pak;
    
    pak.SetType(MP_LOGIN_REQ);
    pak.AddUserPass("ben", "1234");
    pak.Finalize();
    
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
    int input_ctr = 0;
    while (1) // Main loop 
    {
        while (enet_host_service(client.host, &event, 1) > 0)  // is there an event?
        {
            client.ProcessEvent(&event);
        }

        // Other processing (input etc:)
        int a = _getc();
        if(a < 127 && a > -1) {
            input[input_ctr++] = (char)a;
            printf("%d", a);
            // Key-by-key processing here: 


        }
        if(a == 10 || a == 13) { // ? 
            size_t _l = strlen(input);
            printf("\nString got: %s\n", input);
            // String input processing here: 
            

            // reset input 
            clear(&input, 256);
            input_ctr = 0;
        }
    }

    client.Disconnect();// De-init 

    return 0;

}


void clear(void* dat, size_t ct)
{
    char* _v = (char*)dat;
    for(size_t i = 0; i < ct; i++) { _v[i] = '\00'; }
}

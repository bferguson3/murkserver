#include "server.hpp"
#include <cstring>

namespace Murk
{

void Server::ProcessEvent(ENetEvent event)
{

  char *mypacket;
  size_t len;
  Murk::User _nuser;
    switch (event.type)
    {
        case ENET_EVENT_TYPE_CONNECT:
            printf("Client connected from %x:%u.\n",
                    event.peer->address.host, event.peer->address.port);
            
            // Create a unique guid for the user 
            char _guid[16];
            generate_new_guid(&_guid[0]);

            // Allocate a user and copy the guid 
            //Murk::User _nuser;
            _nuser.SetID(&_guid[0]);
            
            activeUserMap[_guid] = _nuser;  // Assign "guid" = MurkUser
            
            //CreatePlayerStruct(event.peer);
            //ENetPacket *packet = enet_packet_create(welcome_packet, strlen(welcome_packet), ENET_PACKET_FLAG_RELIABLE);
            //enet_peer_send(event.peer, 0, packet);
            
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            /* Receive event type */
            printf("[Debug] EVENT RECEIVE\n");
            len = event.packet->dataLength;
            /*
            mypacket = (char *)malloc(len);
            memcpy(mypacket, event.packet->data, len); // Duplicate the packet
            ProcessPacket(mypacket, len, event.peer); // Process it 
            enet_packet_destroy(event.packet); // And destroy them
            free(mypacket);
            */
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", (char *)event.peer->data);
            
            event.peer->data = NULL;
            break;

    }

}


//! Initialize ENet driver
//!
void Server::InitEnet()
{

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Error while initializing ENet.\n");
        exit(EXIT_FAILURE);
    }

    address.host = ENET_HOST_ANY;  // localhost
    address.port = 1234;
    server = enet_host_create(&address, 128, 2, 0, 0);
    if (server == NULL)
    {
        fprintf(stderr, "An error occurred trying to create server\n");
        exit(EXIT_FAILURE);
    }

    // OK!
    printf("MURK server started on localhost:%d \n", address.port);
    return;
}


//! Initialize SQL access to database
//! 
void Server::InitSQL()
{
    int rc;
    rc = sqlite3_open("users.db", &murk_userdb);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(murk_userdb));
        exit(EXIT_FAILURE);
    }
}

}

// murk server.c

#include <enet/enet.h>
#include <sqlite3.h>
#include <stdio.h>

#include "res/base64.h"
#include "res/guid.h"
#include "res/json.h"
#include "murk.h"
#include "server_sql.h"
#include "server.h"

ENetAddress address;
ENetHost *server;

const char *welcome_packet =
    "{\
 \"packetType\":\"LOGIN_WELCOME\",\
 \"text\":\"Welcome to \x1b[31mMUDVERSE!\x1b[0m\",\
 \"tickRate\":250\
}";

char decrypted_pass[128] = {0};
sqlite3 *murk_userdb;
byte password_buffer[128] = {0};
UserState* activePlayerDatabase;

const int PLAYERS_MAX = 512;

const Screen scrMainMenu = { 
    0, 
    MAIN_MENU, 
    no_exits, 
    "Main menu of MURKSERVER."
};


//
int main(int argc, char **argv)
{
    //
    /// database
    //
    int rc;
    rc = sqlite3_open("users.db", &murk_userdb);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n",
                sqlite3_errmsg(murk_userdb));
        return -1;
    }
    //

    //
    /// initialize enet
    //
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Error while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    address.host = ENET_HOST_ANY;  // localhost
    address.port = 1234;
    server = enet_host_create(&address, 128, 2, 0, 0);
    if (server == NULL)
    {
        fprintf(stderr, "An error occurred trying to create server\n");
        exit(EXIT_FAILURE);
    }
    //

    //
    /// Allocate memory for player data 
    //
    printf("Allocating %d bytes for player data, please wait.\n", sizeof(UserState) * PLAYERS_MAX);
    activePlayerDatabase = (UserState*)(malloc(sizeof(UserState) * PLAYERS_MAX));
    for(int i = 0; i < PLAYERS_MAX; i++)
    {
        activePlayerDatabase[i].state = STATE_OFFLINE; 
    }
    //

    // OK!
    printf("MURK server started on localhost:%d with tick rate of %d\n",
           address.port, TICK_RATE);

    //
    /// Main loop:
    //
    ENetEvent event;
    while (1)
    {
        // infinite loop
        while (enet_host_service(server, &event, TICK_RATE) >
               0)  // is there an event?
        {
            ProcessEvent(event);
        }
    }

    // De-init server
    enet_host_destroy(server);
    atexit(enet_deinitialize);
    sqlite3_close(murk_userdb);

    return 0;
}

void CreatePlayerStruct(ENetPeer* peer)
{
    for(int i = 0; i < PLAYERS_MAX; i++)
    {
        if(activePlayerDatabase[i].state == STATE_OFFLINE)
        {
            // this is where we will set the data 
            peer->data = &activePlayerDatabase[i]; // Until the user is disconnected, it will point TO THIS ADDRESS.
            
            char* _g = generate_new_guid();
            g_copy(_g, &activePlayerDatabase[i].id, 16); // copy a guid into the memory block 
            free(_g);
            
            i = PLAYERS_MAX;
        }
    }
}

void ProcessEvent(ENetEvent event)
{
    char *mypacket;
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                /* Connection event */
                printf("Client connected from %x:%u.\n",
                        event.peer->address.host, event.peer->address.port);

                //event.peer->data =
                    //generate_new_guid();  //"Test"; // Generate user ID here
                CreatePlayerStruct(event.peer);

                ENetPacket *packet = enet_packet_create(
                    welcome_packet, strlen(welcome_packet),
                    ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(event.peer, 0, packet);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                /* Receive event type */
                printf("[Debug] EVENT RECEIVE\n");
                size_t len = event.packet->dataLength;
                mypacket = (char *)malloc(len);
                memcpy(mypacket, event.packet->data, len); // Duplicate the packet

                ProcessPacket(mypacket, len, event.peer); // Process it 
                
                enet_packet_destroy(event.packet); // And destroy them
                free(mypacket);
                
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                /* Disconnect event */
                printf("%s disconnected.\n", (char *)event.peer->data);
                
                free(event.peer->data);
                break;

            case ENET_EVENT_TYPE_NONE:

                break;
        }
}

bool bytecmp(void *src, void *dst, size_t count)
{
    unsigned char *p1 = src;
    unsigned char *p2 = dst;
    for (size_t c = 0; c < count; c++)
    {
        if (*p1 != *p2) return 0;
    }
    return 1;  // OK
}

void g_copy(char *src, char *dst, size_t len)
{
    for (int i = 0; i < len; i++) *dst++ = *src++;
}

char* str_append(char* a, char* b)
{
    char* r = (char*)malloc(strlen(a) + strlen(b));
    g_copy(a, r, strlen(a));
    g_copy(b, r+(strlen(a)), strlen(b));
    return r;
}
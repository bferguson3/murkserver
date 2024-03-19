// murk server.c

#include <enet/enet.h>
#include <sqlite3.h>
#include <stdio.h>

#include "base64.h"
#include "guid.h"
#include "json.h"
#include "jsonres.h"
#include "murk.h"
#include "server_sql.h"

ENetAddress address;
ENetHost *server;

const char *welcome_packet =
    "{\
 \"packetType\":\"LOGIN_WELCOME\",\
 \"text\":\"Welcome to \x1b[31mMUDVERSE!\x1b[0m\",\
 \"tickRate\":250\
}";

char decrypted_pass[128] = {0};
s_login user;

// fdefs
void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer);
void g_copy(char *src, char *dst, size_t len);

// local tick rate
#define TICK_RATE 250

sqlite3 *murk_userdb;
byte password_buffer[128] = {0};
size_t last_buffer_size = 0;

typedef struct _exits { 
    bool N;
    bool S;
    bool E;
    bool W;
    bool Up;
    bool Down;
    bool NW;
    bool NE;
    bool SE;
    bool SW;
} Exits;

enum ScreenType { 
    LOGIN_SCREEN = 0,
    MAIN_MENU = 1
};

typedef struct _screen { 
    int id_no;
    enum ScreenType type;
    char description[256];
    Exits exits;
} Screen;

enum PlayerState { 
    STATE_NONE = 0,
    STATE_MAINMENU = 1,
    STATE_IDLE = 2,
    STATE_INCOMBAT = 3,
    STATE_DEAD = 4
};

typedef struct _user_state { 
    Screen* currentScreen; 
    enum PlayerState state;
    char userName[64];
    int level;
    char miscBytes[256];
} UserState;
// 512 at most?

//
int main(int argc, char **argv)
{
    /// database
    int rc;
    rc = sqlite3_open("users.db", &murk_userdb);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n",
                sqlite3_errmsg(murk_userdb));
        return -1;
    }

    // initialize enet
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

    // Allocate memory for player data 
    printf("Allocating %d bytes for player data, please wait.\n", sizeof(UserState) * 512);
    UserState* activePlayerDatabase = (UserState*)(malloc(sizeof(UserState) * 512));

    // OK!
    printf("MURK server started on localhost:%d with tick rate of %d\n",
           address.port, TICK_RATE);

    ENetEvent event;
    while (1)
    {
        // infinite loop
        while (enet_host_service(server, &event, TICK_RATE) >
               0)  // is there an event?
        {
            char *mypacket;
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    /* Connection event */
                    printf("Client connected from %x:%u.\n",
                           event.peer->address.host, event.peer->address.port);

                    event.peer->data =
                        generate_new_guid();  //"Test"; // Generate user ID here

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
                    memcpy(mypacket, event.packet->data, len);
                    ProcessPacket(mypacket, len, event.peer);
                    enet_packet_destroy(event.packet);
                    free(mypacket);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    /* Disconnect event */
                    printf("%s disconnected.\n", (char *)event.peer->data);
                    // event.peer->data = NULL;
                    free(event.peer->data);
                    break;

                case ENET_EVENT_TYPE_NONE:

                    break;
            }
        }
    }

    // De-init server
    enet_host_destroy(server);
    atexit(enet_deinitialize);
    sqlite3_close(murk_userdb);

    return 0;
}

void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer)
{
    printf("[Debug] Packet length: %zu len\n", len);
    printf("%s\n", pkt);

    //  parse json object
    JSONVal j = json_parse(pkt, len);

    // into a struct
    JSONObject object = (JSONObject)j->payload;
    JSONElement p_type = object->start;
    JSONString p_type_str = p_type->name;

    ProcessAction p_act;

    // Packet Type verification
    // error out if element 0 is not packetType (change later)
    if (strcmp(p_type_str->string, "packetType") != 0)
    {
        printf("Error: packetType not found\n");
        return;
    }

    // get the packet type
    JSONVal p_type_val = p_type->value;
    JSONString p_type_valstr = p_type_val->payload;
    // Debug print the packet type
    printf("[Debug] packetType: %s\n", p_type_valstr->string);

    // assign last json element
    JSONElement last_j = p_type;

    // vars we might need:
    bool passwordOK = false;
    bool processLogin = false;
    int userIndex = -1;
    void (*FinalizeFunction)();
    enum MURK_PACKET_TYPES packetType;

    // continue while there are still elements in the blob
    while (last_j->next != json_null)
    {
        // get next element:
        JSONElement next_j = last_j->next;
        JSONString next_name = next_j->name;
        JSONVal next_val = next_j->value;

        //
        if (strcmp(p_type_valstr->string, "LOGIN_REQ") == 0)
        {
            // Login Request packet from user
            //
            // FinalizeFunction = finalizeLoginReq;
            //
            if (JNEXT("user"))  // username
            {
                JSONString next_str = next_val->payload;
                
                p_act = PRA_PROCESSLOGIN;
                user.userName =
                    (char *)next_str->string;  // user_db[i].userName;

                last_buffer_size = GetBlobLength("users", "user_id", "password",
                                                 user.userName);
            }
            else if (JNEXT("pass")) /* handle password field */
            {
                JSONString pw = next_val->payload;
                
                user.password = pw->string;//(char *)&decrypted_pass;
                
            }
        }
        //
        // Other packets
        //
        if(strcmp(p_type_valstr->string, "MENU_SEL") == 0)
        {
            p_act = PRA_MENUSELECT;
            
        }

        last_j = next_j;
    }

    // Final processing loop
    if (p_act == PRA_PROCESSLOGIN)
    {
        if (CheckUserPass(user.userName, (byte *)user.password))
        {
            printf("Login OK!\n");
            // LOGIN SUCCESSFUL !

            ENetPacket *packet =
                enet_packet_create(mainmenu_json, strlen(mainmenu_json),
                                   ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);

            // create a new packet that is a login welcome (make char etc)
        }
        else
        {
            printf("Invalid username or password. Ctrl+C to exit...\n");
        }
    }
    if(p_act == PRA_MENUSELECT)
    {

    }

    free(j);
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

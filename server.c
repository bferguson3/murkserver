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

const int PLAYERS_MAX = 512;

const char *welcome_packet =
    "{\
 \"packetType\":\"LOGIN_WELCOME\",\
 \"text\":\"Welcome to \x1b[31mMUDVERSE!\x1b[0m\",\
 \"tickRate\":250\
}";

static const Screen scrMainMenu = { 
    0, 
    MAIN_MENU, 
    no_exits, 
    "Main menu of MURKSERVER."
};

char decrypted_pass[128] = {0};

s_login user;

// fdefs
void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer);
void ProcessMenu(ENetPeer* peer, Screen* menu, char s);
void g_copy(char *src, char *dst, size_t len);
void SendMessagePacket(const char* msg, ENetPeer* peer);
char* str_append(char* a, char* b);

// local tick rate
#define TICK_RATE 250

sqlite3 *murk_userdb;
byte password_buffer[128] = {0};
size_t last_buffer_size = 0;

UserState* activePlayerDatabase;

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
    printf("Allocating %d bytes for player data, please wait.\n", sizeof(UserState) * PLAYERS_MAX);
    activePlayerDatabase = (UserState*)(malloc(sizeof(UserState) * PLAYERS_MAX));
    for(int i = 0; i < PLAYERS_MAX; i++)
    {
        activePlayerDatabase[i].state = STATE_OFFLINE; 
    }

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
                    // find the user and clear it from ram 
                    //for(int i = 0; i < PLAYERS_MAX; i++){
                    //    if(strcmp(activePlayerDatabase[i].id, (char*)event.peer->data) != -1){
                    //        activePlayerDatabase[i].state = STATE_OFFLINE;
                    //        printf("Removed OK.\n");
                    //        break;
                    //    }
                    //}
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
    //printf("%s\n", pkt);

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
            
            if(JNEXT("value")){
                JSONString next_str = next_val->payload;
                char* _menuSel = (char*)next_str->string;
                
                //printf("next: %s\n", _menuSel);
                
                ProcessMenu(peer, &scrMainMenu, _menuSel[0]);
            }
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
            
            UserState* ps;
            for(int i = 0; i < PLAYERS_MAX; i++){
                if(activePlayerDatabase[i].state == STATE_OFFLINE){
                    ps = &activePlayerDatabase[i];
                    break;
                }
            }
            //ps->id = (char*)(malloc(strlen(peer->data)));
            g_copy(peer->data, ps->id, 16);

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

void ProcessMenu(ENetPeer* peer, Screen* menu, char s)
{
    if(menu->type == MAIN_MENU){
        if(s == '1'){
            SendMessagePacket("New game selected.", peer);
        }
    }
}

void SendMessagePacket(const char* msg, ENetPeer* peer)
{
    const char msg_gen[] = "{\
\"packetType\":\"MESSAGE_GEN\",\
\"message\":\"";
    // msg goes here 
    const char msg_gen2[] = "\"}";

    size_t totsz = strlen(msg_gen) + strlen(msg_gen2) + strlen(msg);
    
    char* newstr = str_append(msg_gen, msg); // alloc
    char* finalstr = str_append(newstr, msg_gen2); //alloc
    free(newstr);

    ENetPacket *packet = enet_packet_create(finalstr, strlen(finalstr), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    
    //printf("%s\n", packet);
    free(finalstr);
}

char* str_append(char* a, char* b)
{
    char* r = (char*)malloc(strlen(a) + strlen(b));
    g_copy(a, r, strlen(a));
    g_copy(b, r+(strlen(a)), strlen(b));
    return r;
}
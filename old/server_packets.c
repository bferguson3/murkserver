//! @file server_packets.c 
//! Packet processing code for the server.
//!

#include <enet/enet.h>
#include <sqlite3.h>
#include <stdio.h>

#include "res/base64.h"
#include "res/guid.h"
#include "res/json.h"
#include "jsonres.h"
#include "murk.h"
#include "server_sql.h"
#include "server.h"

s_login user;
size_t last_buffer_size = 0;
extern const int PLAYERS_MAX;
extern const Screen scrMainMenu;
extern UserState* activePlayerDatabase;

ProcessAction ProcessPacketAction(const char *pkt, size_t len, ENetPeer *peer)
{
    last_buffer_size = 0;
    ProcessAction p_act;
    //  parse json object
    JSONVal j = json_parse(pkt, len);

    // into a struct
    JSONObject object = (JSONObject)j->payload;
    JSONElement p_type = object->start;
    JSONString p_type_str = p_type->name;

    // Packet Type verification
    // error out if element 0 is not packetType (change later)
    if (strcmp(p_type_str->string, "packetType") != 0)
    {
        printf("Error: packetType not found\n");
        return -1;
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
                // add the menusel to this user's "last action value"?
                //printf("next: %s\n", _menuSel);
                
                ProcessMenu(peer, &scrMainMenu, _menuSel[0]);
            }
        }

        last_j = next_j;
    }

    free(j);

    return p_act;
}

void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer)
{
    printf("[Debug] Packet length: %zu len\n", len);
    printf("%s\n", pkt);

    ProcessAction p_act;

    p_act = ProcessPacketAction(pkt, len, peer);

    // Final processing loop
    ProcessPacket_Final(p_act, peer);

}

void ProcessPacket_Final(PacketAction p_act, ENetPeer* peer)
{

    if (p_act == PRA_PROCESSLOGIN)
    {
        //printf("%s\n", user.password);
        if (CheckUserPass(user.userName, (byte *)user.password))
        {
            //printf("Login OK!\n");
            // LOGIN SUCCESSFUL !
            
            
            UserState* ps = &activePlayerDatabase[0];
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

    //printf("%s\n", peer->data);

    ENetPacket *packet = enet_packet_create(finalstr, strlen(finalstr), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    
    //printf("%s\n", packet);
    free(finalstr);
}


#include <enet/enet.h>
#include <stdio.h>
//#include <termios.h>

#include "ansi.h"
#include "res/base64.h"
//#include "res/getpasswd.c"
#include "res/json.h"
#include "murk.h"
#include "res/sha3.h"
#include "client.h"

extern unsigned int currentTickRate;
char pw[64] = { ' ' };
char un[64];

char *loginpkt;
char *menuselpkt;
extern ENetHost* client;
char menuInput = 0;
extern bool reconnect_queued;


void ProcessEvent(ENetEvent* event)
{
    char *mypacket;

    switch (event->type)
    {
        /* Handle connection event */
        case ENET_EVENT_TYPE_CONNECT:
            printf(
                "A packet of length %u containing %s was received from "
                "%s on "
                "channel %u.\n",
                (unsigned int)event->packet->dataLength,
                (char *)event->packet->data, (char *)event->peer->data,
                event->channelID);
            break;

        /* Handle all other events */
        case ENET_EVENT_TYPE_RECEIVE:
            printf("[Debug] EVENT RECEIVE\n");
            // allocate for data
            size_t len = event->packet->dataLength;
            mypacket = (char *)malloc(len);
            // copy it in
            memcpy(mypacket, event->packet->data, len);

            // parse it
            ProcessPacket(mypacket, len, event->peer);

            // delete packet
            enet_packet_destroy(event->packet);
            // free the mem
            free(mypacket);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("server disconnected.\n");
            enet_peer_reset(event->peer);
            reconnect_queued = 1;
            break;

        case ENET_EVENT_TYPE_NONE:
            printf("sfasdf");
            break;
    }
}


void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer)
{
    printf("[Debug] Packet length: %zu len\n", len);
    printf("%s\n", pkt);
    // parse json object
    struct json_value_s *j = json_parse(pkt, len);
    // into a struct
    if(j == 0){
        printf("ERROR\n");
    }
    
    struct json_object_s *object = (struct json_object_s *)j->payload;
    struct json_object_element_s *p_type = object->start;
    struct json_string_s *p_type_str = p_type->name;
    
    // error out if element 0 is not packetType (change later)
    if (strcmp(p_type_str->string, "packetType") != 0)
    {
        printf("Error: packetType not found\n");
        return;
    }

    PacketAction p_act = PA_NONE;

    // get the packet type
    JSONVal p_type_val = p_type->value;
    JSONString p_type_valstr = p_type_val->payload;
    
    // Debug print the packet type
    printf("[Debug] packetType: %s\n", p_type_valstr->string);

    // assign last json element
    JSONElement last_j = p_type;

    // continue while there are still elements in the blob
    while (last_j->next != json_null)
    {
        last_j = last_j->next;
        ProcessPacket_Type(last_j, &p_act, p_type_valstr->string);   
    }

    //
    // Final processing loop
    // After the json is fully parsed, complete the processes here.
    //
    ProcessPacket_Final(p_act, peer);


    free(j);
}


void ProcessPacket_Type(JSONElement next_j, PacketAction* p_act, const char* pType)
{
    // get next element:
    //JSONElement next_j = last_j->next;
    JSONString next_name = next_j->name;
    JSONVal next_val = next_j->value;
    //PacketAction p_act;

    //
    // LOGIN_WELCOME packet
    //
    // This is the first packet received by the server.
    //  In response, the user's login and pw are expected.
    if (strcmp(pType, "LOGIN_WELCOME") == 0)
    {
        //
        *p_act = PA_LOGIN;

        if (JNEXT("text"))
        {
            JSONString next_str = next_val->payload;
            printf("%s\n", next_str->string);
        }
        else if (JNEXT("tickRate"))
        {
            struct json_number_s *j_tr = next_val->payload;
            int tr = atoi(j_tr->number);
            printf("Tick rate requested: %d\n", tr);
            currentTickRate = tr;
        }
        //
    }
    //
    // MENU_MAIN PACKET
    //
    else if (strcmp(pType, "MENU_MAIN") == 0)
    {
        *p_act = PA_MENUSELECT;

        if (JNEXT("text"))
        {
            JSONString next_str = next_val->payload;
            printf("%s\n", next_str->string);
        }
        else if (JNEXT("menuOptions"))
        {
            printf(_Rst);  // Reset ansi just in case
            
            // get list of menu options
            struct json_array_s *array = json_value_as_array(next_val);
            int arlen = array->length;  // and length
            
            // get first element
            struct json_array_element_s *this_ele = array->start;
            JSONString ele_str = json_value_as_string(this_ele->value);
            printf("%s\n", ele_str->string);  // and print it
            for (int c = 1; c < arlen; c++)
            {
                this_ele = this_ele->next;
                ele_str = json_value_as_string(this_ele->value);
                printf("%s\n", ele_str->string);
            }  // and the remaning options
        }
    }
    else if (strcmp(pType, "MESSAGE_GEN") == 0)
    {
        *p_act = PA_NONE;
        if(JNEXT("message")){
            JSONString next_str = next_val->payload;
            printf(": %s\n", next_str->string);
        }
    }
    // other packets

    //last_j = next_j;

    return;
}

void ProcessPacket_Final(PacketAction p_act, ENetPeer* peer)
{

    //
    // LOGIN PACKET PROCESS
    //
    if (p_act == PA_LOGIN)
    {
        char *p = pw;
        FILE *fp = stdin;
        size_t nchr;
        /*
        printf("\nUsername: ");
#ifdef WIN32
        scanf_s("%s", un);
        fflush(stdin);
        printf("Password: ");
        scanf_s("%s", pw);
#else
        scanf("%s", un);
        fflush(stdin);
        printf("Password: ");
        scanf("%s", pw);
#endif 
        */
        //nchr = getpasswd(&p, 64, '*', fp);

        void *myhash = encrypt_pass(pw);

        loginpkt = ConstructLoginPacket(un, myhash);

        printf("\nLogging in...\n");
        printf("%s\n", loginpkt);
        ENetPacket *packet = enet_packet_create(loginpkt, strlen(loginpkt),
                                                ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        free(loginpkt);
        // enet_packet_destroy(packet); << dont destroy this yet... hmm
    }
    else if (p_act == PA_MENUSELECT)
    {
        // then ask for input
        menuInput = CmdPrompt("> ")[0];  // only take the 1st character
        
        menuselpkt = ConstructMenuPacket(menuInput);
        
        ENetPacket *pak = enet_packet_create(menuselpkt, strlen(menuselpkt), 
            ENET_PACKET_FLAG_RELIABLE);
        
        enet_peer_send(peer, 0, pak);
        
        free(menuselpkt);
    }

}


char* ConstructMenuPacket(char sel)
{
    // defining strings as const and copying them into memory is SAFE.
    const char* pakex = "{\"packetType\":\"MENU_SEL\",\n\"value\":\" \"\n}";
    size_t totalsz = strlen(pakex);
    char* menuPak = (char*)malloc(totalsz);
    g_copy(pakex, menuPak, totalsz);
    menuPak[35] = sel;
    return menuPak;
}

char *ConstructLoginPacket(char *un, void *pw)
{
    // sample:
    const char loginpacketex[] =
        "{\"packetType\":\"LOGIN_REQ\",\n\"user\":\"\",\n\"pass\":\"\"\
}";

    // get size of packet and allocate it, pw is 512 hash
    int enclen = Base64encode_len(512 / 8);

    size_t totalsz = strlen(un) + enclen + strlen(loginpacketex);
    
    char *loginPacket = (char *)malloc(totalsz);

    int i;
    int ulen = strlen(un);
    
    int len_p;
    const int ALEN = 35;
    const int BLEN = 11;
    g_copy((char *)loginpacketex, loginPacket, ALEN);

    // copy in UN and PW
    for (i = 0; i < ulen; i++) loginPacket[ALEN + i] = un[i];  // username
    // next 10 chars
    for (i = (i + ALEN); i < (ALEN + ulen + BLEN); i++)
    {
        loginPacket[i] = loginpacketex[i - ulen];
    }

    char *encodedpass = (char *)malloc(enclen);
    Base64encode(encodedpass, pw, 512 / 8);

    //  password
    for (int n = 0; n < enclen - 1; n++)
    {
        loginPacket[i++] = encodedpass[n];
    }

    // end
    loginPacket[i++] = '"';
    loginPacket[i++] = '}';
    loginPacket[i++] = '\x00';

    //printf("\n%s\n", loginPacket);

    return loginPacket;
}

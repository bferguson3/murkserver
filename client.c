// #include <cstdio.h>
#include <enet/enet.h>
#include <stdio.h>
#include <termios.h>

#include "ansi.h"
#include "base64.h"
#include "getpasswd.c"
#include "json.h"
#include "murk.h"
#include "sha3.h"

ENetHost *client;
unsigned int currentTickRate;

char *loginpkt;
char *menuselpkt;

char un[64];
char pw[64] = {0};
char lastInput[64];

typedef enum packet_action
{
    PA_NONE = 0,
    PA_LOGIN = 1,
    PA_MENUSELECT = 2
} PacketAction;

char menuInput = 0;

//

char *ConstructLoginPacket(char *un, void *pw);
void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer);
ssize_t getpasswd(char **pw, size_t sz, int mask, FILE *fp);
void g_copy(char *src, char *dst, size_t len);
void *encrypt_pass(char *pass);

// void *hash;

void assert(bool tf)
{
    if (!tf)
    {
        printf("*ASSERTION FAILED*\n");
        abort();
    }
}

const char *CmdPrompt(const char *prompt)
{
    printf("%s", prompt);
    fflush(stdout);
    scanf("%s", lastInput);
    return (const char *)&lastInput[0];
    // printf("[Debug] Last Input: %s\n", lastInput);
}

//
int main(int argc, char **argv)
{
    printf(_Cls _FG_BYellow _SetPos(
        10, 10) "TEST"
                "\n");

    // setup and init:

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Error while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    client = enet_host_create(NULL, 1, 2, 0, 0);
    if (client == NULL)
    {
        fprintf(stderr, "An error occurred trying to create client.\n");
        exit(EXIT_FAILURE);
    }

    ENetAddress address;
    ENetEvent event;
    ENetPeer *peer;

    // localhost target for testing
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL)
    {
        fprintf(stderr, "No peers to connect... Bad address?\n");
        exit(EXIT_FAILURE);
    }

    // wait 5 seconds for the connection event
    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connection to localhost:1234 succeeded.");
    }
    else
    {
        enet_peer_reset(peer);
        puts("Connection failed\n");
    }

    //
    //
    // OK! Main loop
    //
    //
    while (1)
    {
        while (enet_host_service(client, &event, currentTickRate) > 0)
        {
            char *mypacket;

            switch (event.type)
            {
                /* Handle connection event */
                case ENET_EVENT_TYPE_CONNECT:
                    printf(
                        "A packet of length %u containing %s was received from "
                        "%s on "
                        "channel %u.\n",
                        (unsigned int)event.packet->dataLength,
                        (char *)event.packet->data, (char *)event.peer->data,
                        event.channelID);
                    break;

                /* Handle all other events */
                case ENET_EVENT_TYPE_RECEIVE:
                    printf("[Debug] EVENT RECEIVE\n");
                    // allocate for data
                    size_t len = event.packet->dataLength;
                    mypacket = (char *)malloc(len);
                    // copy it in
                    memcpy(mypacket, event.packet->data, len);

                    // parse it
                    ProcessPacket(mypacket, len, event.peer);

                    // delete packet
                    enet_packet_destroy(event.packet);
                    // free the mem
                    free(mypacket);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("%s disconnected.\n", (char *)event.peer->data);
                    break;

                case ENET_EVENT_TYPE_NONE:
                    printf("sfasdf");
                    break;
            }
        }
    }

    //
    //
    // End connection trail:
    //
    //
    enet_peer_disconnect(peer, 0);
    uint8_t disc = 0;
    while (enet_host_service(client, &event, 3000) > 0)
    {
        switch (event.type)
        {  // client packet type
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                puts("Disconnect ok\n");
                disc = 1;
                break;
        }
    }
    // if disconnect bool is false
    if (!disc) enet_peer_reset(peer);

    enet_host_destroy(client);
    atexit(enet_deinitialize);
    //
    return 0;
}

//
//
void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer)
{
    printf("[Debug] Packet length: %zu len\n", len);

    // parse json object
    struct json_value_s *j = json_parse(pkt, len);

    // into a struct
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
    // printf("[Debug] packetType: %s\n", p_type_valstr->string);

    // assign last json element
    JSONElement last_j = p_type;
    // continue while there are still elements in the blob
    while (last_j->next != json_null)
    {
        // get next element:
        JSONElement next_j = last_j->next;
        JSONString next_name = next_j->name;
        JSONVal next_val = next_j->value;

        //
        // LOGIN_WELCOME packet
        //
        // This is the first packet received by the server.
        //  In response, the user's login and pw are expected.
        if (strcmp(p_type_valstr->string, "LOGIN_WELCOME") == 0)
        {
            //
            p_act = PA_LOGIN;

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
        else if (PACKETTYPEIS(MENU_MAIN))
        {
            p_act = PA_MENUSELECT;

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

                // then ask for input
                menuInput = *CmdPrompt("> ");  // only take the 1st character

                // printf("[Debug] The first character you input is: %c\n",
                //        menuInput);
            }
        }
        // other packets

        last_j = next_j;
    }

    //
    // Final processing loop
    // After the json is fully parsed, complete the processes here.
    //

    if (p_act == PA_LOGIN)
    {
        char *p = pw;
        FILE *fp = stdin;
        size_t nchr;

        printf("\nUsername: ");
        scanf("%s", un);
        fflush(stdin);
        printf("Password: ");
        nchr = getpasswd(&p, 64, '*', fp);

        void *myhash = encrypt_pass(pw);

        // printf("%s", myhash);
        loginpkt = ConstructLoginPacket(un, myhash);

        printf("\nLogging in...\n");

        ENetPacket *packet = enet_packet_create(loginpkt, strlen(loginpkt),
                                                ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        free(loginpkt);
        // enet_packet_destroy(packet); << dont destroy this yet... hmm
    }
    else if (p_act == PA_MENUSELECT)
    {
        //
    }

    free(j);
}

unsigned char temp_pass_sh[64] = {0};

void *encrypt_pass(char *pass)
{
    // sha3 test
    sha3_context c;
    uint8_t *hash;
    sha3_Init512(&c);
    sha3_Update(&c, pass, strlen(pass));
    hash = sha3_Finalize(&c);
    // for (int i = 0; i < 64; i++) printf("%u ", hash[i]);
    g_copy(hash, temp_pass_sh, 64);

    return &temp_pass_sh[0];
}

void g_copy(char *src, char *dst, size_t len)
{
    for (int i = 0; i < len; i++) *dst++ = *src++;
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
    // printf("size %d", enclen);
    char *loginPacket = (char *)malloc(totalsz);

    int i;
    int ulen = strlen(un);
    // int plen = 512 / 8;
    int len_p;
    const int ALEN = 35;
    const int BLEN = 11;
    g_copy(loginpacketex, loginPacket, ALEN);

    // copy in UN and PW
    for (i = 0; i < ulen; i++) loginPacket[ALEN + i] = un[i];  // username
    // next 10 chars
    for (i = (i + ALEN); i < (ALEN + ulen + BLEN); i++)
    {
        loginPacket[i] = loginpacketex[i - ulen];
    }

    char *encodedpass = (char *)malloc(enclen);
    Base64encode(encodedpass, pw, 512 / 8);

    // char *oldp = &pw[0];
    // for (i = 0; i < 64; i++) printf("%d ", *oldp++);

    //  password
    for (int n = 0; n < enclen - 1; n++)
    {
        loginPacket[i++] = encodedpass[n];
    }

    // end
    loginPacket[i++] = '"';
    loginPacket[i++] = '}';

    printf("\n%s\n", loginPacket);

    return loginPacket;
}

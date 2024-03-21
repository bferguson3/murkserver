// murk client.c

#include <enet/enet.h>
#include <stdio.h>
#include <termios.h>

#include "ansi.h"
#include "base64.h"
#include "json.h"
#include "murk.h"
#include "sha3.h"
#include "client.h"

ENetHost *client;
unsigned int currentTickRate;

char lastInput[64];
unsigned char temp_pass_sh[64] = {0};

//
int main(int argc, char **argv)
{
    //
    //
    // setup and init ENet:
    //
    //
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
            ProcessClientPacket(&event);
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
            case ENET_EVENT_TYPE_NONE:
                break;
            case ENET_EVENT_TYPE_CONNECT:
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

void *encrypt_pass(char *pass)
{
    // sha3 test
    sha3_context c;
    void *hash;
    sha3_Init512(&c);
    sha3_Update(&c, pass, strlen(pass));
    hash = (void *)sha3_Finalize(&c);

    g_copy((char *)hash, (char *)temp_pass_sh, 64);

    return &temp_pass_sh[0];
}

void g_copy(char *src, char *dst, size_t len)
{
    for (int i = 0; i < len; i++) *dst++ = *src++;
}

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

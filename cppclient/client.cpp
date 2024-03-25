// client.cpp 
#include "client.hpp"
#include <cstdio>
#include <fcntl.h>

#include "packet.hpp"

extern int _getc();
extern void clear(void* a, size_t s);

namespace Murk
{

void Client::InitEnet()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Error while initializing ENet.\n");
        exit(EXIT_FAILURE);
    }

    host = enet_host_create(NULL, 1, 2, 0, 0);
    if (host == NULL)
    {
        fprintf(stderr, "An error occurred trying to create client.\n");
        exit(EXIT_FAILURE);
    }

    printf("ENet initialized OK.\n");

}


void Client::Connect(ENetAddress* address)
{

    server = enet_host_connect(host, address, 2, 0);
    if (server == NULL)
    {
        fprintf(stderr, "No peers to connect... Bad address?\n");
        exit(EXIT_FAILURE);
    }
    ENetEvent event;
    // wait 5 seconds for the connection event
    if (enet_host_service(host, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("Connected to %x:%d.\n", address->host, address->port);
    }
    else
    {
        enet_peer_reset(server);
        puts("Connection timed out. Is the server online?\n Quitting...\n");
        exit(EXIT_FAILURE);
    }

    
}


void Client::Disconnect()
{
    enet_host_destroy(host);
    atexit(enet_deinitialize);
}


void Client::ProcessEvent(ENetEvent* event)
{

    char *mypacket;

    switch (event->type)
    {
    case ENET_EVENT_TYPE_CONNECT:
        printf ("A new client connected from %x:%u.\n", 
                event->peer -> address.host,
                event->peer -> address.port);
        event->peer -> data = (char*)"Client information";
        break;
    case ENET_EVENT_TYPE_RECEIVE:
        printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                event->packet -> dataLength,
                event->packet -> data,
                event->peer -> data,
                event->channelID);
        enet_packet_destroy (event->packet);
        break;
       
    case ENET_EVENT_TYPE_DISCONNECT:
        printf ("%s disconnected.\n", event->peer -> data);
        event->peer -> data = NULL;
    }

}


void Client::SetNonblocking()
{
    //    Set terminal to raw   
    struct termios tty;
    int r;
    
    r = tcgetattr(STDIN_FILENO, &old_terminal_settings); // save old settings 
    r = tcgetattr(STDIN_FILENO, &tty);
    if (r == -1) { err(1, "tcgetattr failed"); }
    
    tty.c_lflag &= ~(ICANON); // disable canonical input 
    tty.c_lflag &= ~(ECHO);   // disable echo 

    r = tcsetattr(STDIN_FILENO, 0, &tty);
    if (r == -1) { err(1, "tcsetattr failed"); }

    // Set STDIN to non-blocking 
    r = fcntl(STDIN_FILENO, F_GETFL);
    if (r == -1)
        err(1, "F_GETFL");
    r = fcntl(STDIN_FILENO, F_SETFL, r | O_NONBLOCK);
    if (r == -1)
        err(1, "F_SETFL");
   
}


void Client::ProcessInput(char* input)
{
    int a = _getc();

    // Standard check: 
    if(a < 127 && a > -1) {
        input[input_ctr++] = (char)a;
        printf("%d", a);
        // Key-by-key processing here: 


    }
    if(a == 10 || a == 13) { // 10 or 13 depending on mode
        size_t _l = strlen(input);
        printf("\nString got: %s\n", input);
        // String input processing here: 
        

        // reset input 
        clear(input, 256);
        input_ctr = 0;
    }

    // reverse order to get arrows: (MACOS)
    if(arrow_get){
        if(a == 65){
            printf("up");
        } else if(a == 66) {
            printf("down");
        } else if(a == 67) {
            printf("right");
        } else if(a == 68) {
            printf("left");
        }
        arrow_get = false;
    }
    if(arrow_check){
        if(a == 91){
            arrow_get = true;
        }
        arrow_check = false;
    }
    if(a == 27) {
        arrow_check = true;
    } 
    //
    
}


void Client::err(int fi, const char* str)
{
    printf(str);
    exit(fi);
}

}
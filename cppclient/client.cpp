// client.cpp 
#include "client.hpp"
#include <cstdio>
#include <fcntl.h>

#include "packet.hpp"

extern "C" {
    #include "../res/base64.c"
    #include "../res/sha3.c"
    #include <enet/enet.h>
}

extern int _getc();
extern void clear(void* a, size_t s);

namespace Murk
{

Client::Client()
{
    last_c = 0;
    arrow_check = false;
    arrow_get = false;
    FLAG_INPUT_USER = false;
    FLAG_INPUT_PASSWORD = false;
}


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


void Client::GameLoop()
{
    switch(state)
    {
        case STATE_DEAD:
            break;
        case STATE_IDLE:
            break;
        case STATE_INCOMBAT:

            break;
        /////
        case STATE_LOGGING_IN:
            if(!FLAG_INPUT_USER && (userpass.user == "")){
                FLAG_INPUT_USER = true;
                printf("\nUsername: ");
            }
            if(!FLAG_INPUT_PASSWORD && (userpass.user != "") && userpass.pass == ""){
                FLAG_INPUT_PASSWORD = true;
                printf("\nPassword: ");
            }
            break;
        ////
        case STATE_MAINMENU:
            break;

        case STATE_OFFLINE:
            break;
    }
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
        // Key-by-key processing here: 
        if(a > 0x1f && a < 0x80) // Echo normal ascii 
        {
            if(FLAG_INPUT_PASSWORD)
                putc((int)'*', stdout);
            else
                putc(a, stdout); //printf("%c", a); 
        }

    }
    if(a == 10 || a == 13) { // 10 or 13 depending on mode submits
        size_t _l = strlen(input);
        input[_l - 1] = (char)0; // null the RETURN byte at the end
        //printf("\nString got: %s\n", input);

        // String input processing here: 
        if(FLAG_INPUT_USER) {
            userpass.user = input;
            FLAG_INPUT_USER = false;
        }
        if(FLAG_INPUT_PASSWORD) {
            userpass.pass = input;
            FLAG_INPUT_PASSWORD = false;
            
            SendLogin();

            printf("\nLogging in...\n");
        }

        // reset input 
        clear(input, 256);
        input_ctr = 0;
    }

    // special chars: 
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

void Client::SendLogin()
{
    class Murk::Packet pak(MP_LOGIN_REQ);
    
    size_t enclen = Base64encode_len(512 / 8);
    char *encodedpass = (char *)malloc(enclen);
    
    Base64encode(encodedpass, (const char*)Encrypt(userpass.pass.c_str()), 512 / 8);
    userpass.pass = encodedpass;
    
    pak.UserPass(userpass.user, userpass.pass);
    if(pak.Validate() != 0) err(1, "Fatal error: Made a bad json packet...");

    ENetPacket *ep = enet_packet_create(pak.GetString().c_str(), pak.GetString().length(), 
        ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(server, 0, ep);

}

void* Client::Encrypt(const char* dat)
{
    // sha3 test
    sha3_context c;
    void *hash;
    sha3_Init512(&c);
    sha3_Update(&c, dat, strlen(dat));
    hash = (void *)sha3_Finalize(&c);
    //memcpy(hash, userpass.pass, 64);
    //g_copy((char *)hash, (char *)temp_pass_sh, 64);

    return hash;
}


void Client::err(int fi, const char* str)
{
    printf(str);
    exit(fi);
}

}
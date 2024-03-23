// murk client.c

#include <enet/enet.h>
#include <stdio.h>
#include <termios.h>
//#include <termios.h>

#include "ansi.h"
#include "res/base64.h"
#include "res/json.h"
#include "murk.h"
#include "res/sha3.h"
#include "client.h"

ENetHost *client;
unsigned int currentTickRate;
extern char un[64];
extern char pw[64];

char lastInput[64];
unsigned char temp_pass_sh[64] = {0};
bool reconnect_queued;
//#define WIN32 1
//

ENetAddress address;
ENetEvent event;
ENetPeer *peer;

int mygetc() {
    char ch;
    int error;
    static struct termios Otty, Ntty;

    fflush(stdout);
    tcgetattr(0, &Otty);
    Ntty = Otty;

    Ntty.c_iflag  =  0;     /* input mode       */
    Ntty.c_oflag  =  0;     /* output mode      */
    Ntty.c_lflag &= ~ICANON;    /* line settings    */

#if 0
    /* disable echoing the char as it is typed */
    Ntty.c_lflag &= ~ECHO;  /* disable echo     */
#else
    /* enable echoing the char as it is typed */
    Ntty.c_lflag |=  ECHO;  /* enable echo      */
#endif

    Ntty.c_cc[VMIN]  = 0;    /* minimum chars to wait for */
    Ntty.c_cc[VTIME] = 1;   /* minimum wait time    */

#if 0
    /*
    * use this to flush the input buffer before blocking for new input
    */
    #define FLAG TCSAFLUSH
#else
    /*
    * use this to return a char from the current input buffer, or block if
    * no input is waiting.
    */
    #define FLAG TCSANOW

#endif

    if ((error = tcsetattr(0, FLAG, &Ntty)) == 0) 
    {
        error = read(0, &ch, 1 );        /* get char from stdin */
        error += tcsetattr(0, FLAG, &Otty);   /* restore old settings */
    }
    //tcsetattr(0, FLAG, &Otty);
    return (error == 1 ? (int) ch : -1 );
}


void ConnectToServer()
{
    client = enet_host_create(NULL, 1, 2, 0, 0);
    if (client == NULL)
    {
        fprintf(stderr, "An error occurred trying to create client.\n");
        exit(EXIT_FAILURE);
    }


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

}


///
int main(int argc, char **argv)
{
#ifdef WIN32
    HANDLE hStdout, hStdin;
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD fdwOldMode;
    GetConsoleMode(hStdout, &fdwOldMode);
    fdwOldMode = fdwOldMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hStdout, &fdwOldMode);
#endif

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

    reconnect_queued = 0;
    ConnectToServer();
    //
    //
    // OK! Main loop
    //
    //
    char* input = (char*)malloc(256);
    for(int i = 0; i < 256; i++) input[i] = '\00';
    int input_ctr = 0;
    while (1)
    {   
        while (enet_host_service(client, &event, 25) > 0)
        {
            ProcessEvent(&event);
            //enet_peer_ping(peer);
        }
        int a = mygetc();
        if(a < 127 && a > 0x1f) {
            //printf("%c",a);
            input[input_ctr++] = (char)a;
        }
        if(a == 13) {
            //printf("\n%s\n", input);
            printf("\nString got: %s\n", input);
            for(int i = 0; i < 256; i++) {
                input[i] = '\00';
            }
            input_ctr = 0;
        }
        //printf("\n");
        if(reconnect_queued){
            enet_host_destroy(client);
            reconnect_queued = 0;
            ConnectToServer();
        }
        //}
        
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
#ifdef WIN32
    scanf_s("%s", lastInput);
#else
    scanf("%s", lastInput);
#endif
    return (const char *)&lastInput[0];
    // printf("[Debug] Last Input: %s\n", lastInput);
}

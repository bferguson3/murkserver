// client.cpp 
#include "client.hpp"
#include <iostream>


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

    verb_context = verb_context[0];
    //class Screen _s;
    //currentScreen = _s;
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
            if((FLAG_INPUT_USER == false) && (userpass.user == "")){
                FLAG_INPUT_USER = true;
                printf("\nUsername: ");
                input_ctr = 0;
            }
            if(!FLAG_INPUT_PASSWORD && (userpass.user != "") && userpass.pass == ""){
                FLAG_INPUT_PASSWORD = true;
                printf("\nPassword: ");
                input_ctr = 0;
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

    Murk::Packet incoming((char*)event->packet->data);
    
    switch (event->type)
    {
    case ENET_EVENT_TYPE_CONNECT:
        printf ("A new client connected from %x:%u.\n", 
                event->peer -> address.host,
                event->peer -> address.port);
        event->peer -> data = (char*)"Client information";
        break;
    case ENET_EVENT_TYPE_RECEIVE:
        printf ("A packet of length %u was received from %s on channel %u.\n",
                (unsigned int)event->packet -> dataLength,
                (const char*)event->peer -> data,
                event->channelID);
        
        if(incoming.Validate()==0)
        {
            ///
            ProcessPacket(incoming);   // < main packet processing function 
            ///
        }
        else { 
            printf("Error: Failed to validate incoming packet");
        }
        enet_packet_destroy (event->packet);
        break;
       
    case ENET_EVENT_TYPE_DISCONNECT:
        printf ("%s disconnected.\n", (char*)event->peer -> data);
        event->peer -> data = NULL;
        break;
    case ENET_EVENT_TYPE_NONE:
        break;

    }


}


void Client::ProcessPacket(Murk::Packet p)
{
    std::string _t = "type";
    std::string _tx = "text";
    std::string _d = p.GetData(_t);

    if(_d == "MENU_MAIN"){
        // Print the menu text 
        AnsiPrint(p.GetData(_tx));
        
        // And all the options from the blob 
        std::set<std::string> _list = p.GetOptions();
        std::set<std::string>::iterator it;
        for(it = _list.begin(); it != _list.end(); ++it) AnsiPrint((std::string)*it);
        //for(std::string s : _list) {
            //_i ++;
        //    AnsiPrint(s);
        //}
        printf("\n");
        printf(MENU_SELECT_STRING);
        
        //Exits _e;
        //class Screen _s(SCR_MAIN_MENU, _e, "Main Menu");
        //currentScreen = _s;
        // Instead of assigning here, receive the screen data from the server! 
        
        // Set the input flag 
        FLAG_INPUT_MENU = true;

    }
}



/// Stores Ansi codes in buffer to flush at once
/// otherwise, prints one character at a time  
void Client::AnsiPrint(std::string s)
{
    printf("\x1b[0m" "\n"); // ANSI RESET 
    int len = s.length();
    int i = 0;
    while(i < len)
    {
        char p = s[i];
        if(p == '^')        // CUSTOM JSON ESCAPE CODE
        {
            int _ii = i + 1;
            std::string _ts;
            while (s[_ii] != 'm' && s[_ii] != 'h') { // TODO: m and h are 80% of ANSI codes
                _ts += s[_ii];
                _ii++;
            }
            _ts += s[_ii];  // grab last char
            
            printf("\x1b%s",_ts.c_str());   // print ESC+string
            i = _ii;
        }
        else { 
            printf("%c", p); // else just the char
        }
        i++;
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


void Client::SendMenuSelect(char s)
{
    class Murk::Packet pak(MP_MENUSEL);
    pak.Select(s);
    
    if(pak.Validate() != 0) err(1, "Fatal error: Made a bad json packet in sendmenuselect()\n");

    ENetPacket *ep = enet_packet_create(pak.GetString().c_str(), pak.GetString().length(), 
        ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(server, 0, ep);
}


void Client::ProcessInput(char* input)
{
    int a = _getc();
    //if(a > 0) printf("%d\n", a);
    // Standard check: 
    if(a < 127 && a > -1) {
        input[input_ctr++] = (char)a;
        // Key-by-key processing here: 
        if(a > 0x1f && a < 0x80) // Echo normal ascii 
        {
            if(FLAG_INPUT_PASSWORD)
                putc((int)'*', stdout);
            else if(FLAG_MUTE_INPUT)
            { ; }
            else
                putc(a, stdout); 
        }
        if(FLAG_INPUT_MENU){
            // send immediately 
            FLAG_INPUT_MENU = false;
            FLAG_MUTE_INPUT = true;
            SendMenuSelect(a);
            printf("\n"); // CR 
        }
    }
    if(a == 10 || a == 13) { // 10 or 13 depending on mode submits
        size_t _l = strlen(input);
        input[_l - 1] = (char)0; // null the RETURN byte at the end
        
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
    // TAB FOR AUTO COMPLETE 
    if(a == 9) // TAB 
    {
        if(verb_context == "NONE"){

        }
    }
    // DELETE 
    if(a == 127) {
        input_ctr--;
        input[input_ctr] = 0;
        printf("\x1b[1D \x1b[1D");
        std::cout << std::flush;
    }

    // special chars: 
    // reverse order to get arrows: ^[A etc
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
    printf("%s", str);
    exit(fi);
}

}
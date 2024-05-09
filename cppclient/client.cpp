// client.cpp 
#include "client.hpp"
#include <iostream>


extern "C" {
    #include "../res/base64.c"
    #include "../res/sha3.c"
    #include <enet/enet.h>
}

//#include <algorithm>

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

    verb_context = "";//default_context[0];
    //class Screen _s;
    //currentScreen = _s;
    last_action_time.tv_sec = 0;
    last_action_time.tv_nsec = 0;
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
        case STATE_NORMAL:
            
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
        
        printf("\n");
        printf("%s", MENU_SELECT_STRING);
        
        // Set the input flag 
        FLAG_INPUT_MENU = true;

    }
    else if (_d == "MESSAGE_SCREEN"){
        // Print the menu text 
        AnsiPrint(p.GetData(_tx));

        //TODO 
    }
    else if(_d == "ROOM_INFO")
    {
        // [ Room ]
        AnsiPrint("[" + p.GetData("name") + "]");
        // [ Description ]
        AnsiPrint(p.GetData("desc"));
        // Exits
        std::string ex = "There are exits to the ";
        int _ei = stoi(p.GetData("exits"));
        int _l = 0;
        if(_ei & (1)){ 
            ex += "north, ";
            _l = 1;
        }
        if(_ei & (1<<1)){ 
            ex += "south, ";
            _l = (1<<1);
        }
        if(_ei & (1<<2)){ 
            ex += "east, ";
            _l = (1<<2);
        }
        if(_ei & (1<<3)){ 
            ex += "west, ";
            _l = (1<<3);
        }
        if(_ei & (1<<4)){ 
            ex += "up, ";
            _l = (1<<4);
        }
        if(_ei & (1<<5)){ 
            ex += "down, ";
            _l = (1<<5);
        }
        // TODO : ENGLISH 
        if(_l == (1))
            ex = ex.substr(0, ex.length() - 7) + "north.";
        if(_l == (1 << 1))
            ex = ex.substr(0, ex.length() - 7) + "and south.";
        if(_l == (1 << 2))
            ex = ex.substr(0, ex.length() - 6) + "and east.";
        if(_l == (1 << 3))
            ex = ex.substr(0, ex.length() - 6) + "and west.";
        if(_l == (1 << 4))
            ex = ex.substr(0, ex.length() - 4) + "and up.";
        if(_l == (1 << 5))
            ex = ex.substr(0, ex.length() - 6) + "and down.";

        AnsiPrint(ex);

        AnsiPrint(InputPrompt());

        state = STATE_NORMAL;
        FLAG_MUTE_INPUT = false;
    }
}

std::string Client::InputPrompt()
{

    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    
    if(start.tv_sec <= last_action_time.tv_sec) { 
        sleep(1);
    }
    

    last_action_time.tv_sec = start.tv_sec;
    last_action_time.tv_nsec = start.tv_nsec;
    
    return "HP:" + std::to_string(curHP) + "> ";
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

void Client::Capitalize(char* input)
{
    int si = 0;
    while (input[si] != 0x00)
    {
        input[si] = toupper(input[si]);
        si++;
    }
    
}

// find the first verb only and capitalize it 
// Only works for single commands 
void Client::CapitalizeVerb(std::string* s)
{
    std::string p = *s;
    // 1 convert temp to capitals 
    std::string::iterator c;
    for(c = p.begin(); c != p.end(); c++) 
        { *c = toupper(*c); }

    if (p == "S") {
        *s = MURK_VERB_SOUTH;
        return;
    }
    if (p == "E") {
        *s = MURK_VERB_EAST;
        return;
    }
    if (p == "W") {
        *s = MURK_VERB_WEST;
        return;
    }
    if (p == "N") {
        *s = MURK_VERB_NORTH;
        return;
    }
    if (p == "U") {
        *s = MURK_VERB_UP;
        return;
    }
    if (p == "D") {
        *s = MURK_VERB_DOWN;
        return;
    }
    // 2 find verb 
    for(int i = 0; i < DEFAULT_CONTEXT_SIZE; i++){
        ssize_t len = 0;
        
        if(default_context[i].length() > s->length()) len = s->length();
        else len = default_context[i].length();

        if(default_context[i].compare(0, len, p.c_str()) == 0){
        //if(p.compare(0, len, default_context[i].c_str()) == 0){
            *s = default_context[i];
        }
    }
}


void Client::ProcessInput(char* input)
{
    int a = _getc();
    //if(a > 0) printf("%d\n", a);
    // Standard check: 
    if((a < 127) && (a > 0x19) && (a != 9)) {
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
            clear(input, 256);
            input_ctr = 0;
        }
    }
    if(a == 10 || a == 13) { // 10 or 13 depending on mode submits

        //size_t _l = strlen(input);
        //input[_l - 1] = (char)0; // null the RETURN byte at the end
        
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
        if(state == STATE_NORMAL) // Out of combat entity 
        {
            if(verb_context == "") // no context yet? 
            {
                std::string _i; 
                _i = input;
                //printf("[DEBUG] input string now: %s\n", _i.c_str());
                // TODO: loc stuff 
                // get only the first word 
                std::string word = _i.substr(0, _i.find(" "));

                CapitalizeVerb(&word);
                _i.replace(0, _i.find(" "), word);
                
                // search for first verb and use that 
                int i;
                for(i = 0; i < DEFAULT_CONTEXT_SIZE; i++){
                    if(word.find(default_context[i]) != std::string::npos){
                        verb_context = default_context[i];
                        printf("[DEBUG] Context set to %s\n", default_context[i].c_str());
                        break;
                    }
                }

                ssize_t n = _i.find(default_context[i]);
                if(n != std::string::npos) { 
                    _i.erase(n, default_context[i].size());
                }
                strcpy(input, _i.c_str());
            }

            if(verb_context == MURK_VERB_SAY)
            {
                // send msg everything after say 
                std::string words = input;
                std::string ar = MURK_VERB_SAY;
                // if its there, erase it 
                ssize_t n = words.find(ar);
                if (n != std::string::npos){
                    int j = ar.length();
                    words.erase(n, j);
                    // trim forespace
                    if(words.compare(0, 1, " ") != std::string::npos) words.erase(0, 1);
                }

                SendSayMessage(words);
            }
            else
                if(verb_context != "")
                    SendCommand(verb_context);
                else { 
                    printf("\n\x1b[37m Sorry, I didn't understand that.");
                }

            // newline while wait 
            printf("\n");

            AnsiPrint(InputPrompt());
            verb_context = "";
        }
        
        // reset input 
        clear(input, 256);
        input_ctr = 0;
    }
    // TAB FOR AUTO COMPLETE 
    if(a == 9) // TAB 
    {
        // No verb context yet? 
        if(verb_context == ""){
            // This is going to be a lengthy process where we search for the first match in the contexts given
            std::string caps;// = toupper(input);
            Capitalize(input);
            caps = input;
            
            for(int i = 0 ; i < DEFAULT_CONTEXT_SIZE; i++)
            {
                if(default_context[i].compare(0, caps.length(), caps) == 0)
                {
                    // Auto-complete: 
                    std::string ans = "\x1b[" + std::to_string(input_ctr) + "D";
                    std::cout << ans;
            
                    verb_context = default_context[i];
                    
                    strcpy(input, verb_context.c_str());
                    std::cout << input;
                    
                    input_ctr = verb_context.length();

                    break;
                }
            }
        }
        else if(verb_context == MURK_VERB_ATTACK) // loc 
        {
            // if there is anything to attack, 
            // (players only! pvp custom code here!)
            // add a space if needed
            // and then the name of the monster 
        }
        
    }
    // DELETE 
    if(a == 127) {
        if(input_ctr > 0){
            input_ctr--;
            input[input_ctr] = 0;
            
            printf("\x1b[1D \x1b[1D");
            std::cout << std::flush;
            // todo: smart context changes 
            if(input_ctr == 0) verb_context = "";
        }
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

void Client::SendCommand(std::string cmd)
{
    Murk::Packet p(MP_PCOMMAND);
    // TODO : 
    // parse string here: throw away anything not in  
    // - default context 
    // - user context (inventory, scene)

    p.AddCommand(cmd);
    p.AddTarget(MURK_TARGET_SELF);
    p.AddSubject("");

    if(!!p.Validate())
    {
        err(1, "Couldnt validate JSON packet, not sending.");
        return;
    }

    ENetPacket *ep = enet_packet_create(p.GetString().c_str(), p.GetString().length(), 
        ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(server, 0, ep);
    //printf("packet: %s\n", p.GetString().c_str());

}


void Client::SendSayMessage(std::string msg)
{
    // make packet 
    Murk::Packet p(MP_PCOMMAND);
    // add say cmd (SAY LOCAL)
    p.AddCommand(MURK_VERB_SAY);
    p.AddTarget(MURK_TARGET_LOCAL);
    // add object (words), adds end bracket
    p.AddSubject(msg);

    if(!!p.Validate()) {
        err(1, "ERROR: Couldn't validate packet, not sending.\n");
        return;
    }

    ENetPacket *ep = enet_packet_create(p.GetString().c_str(), p.GetString().length(), 
        ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(server, 0, ep);
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
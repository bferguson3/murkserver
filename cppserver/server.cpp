#include "server.hpp"
#include <cstring>

#include "../jsonres.h"
#include "screen.hpp"

namespace Murk
{


//! Initialize SQL access to database
//! 
void Server::InitSQL()
{
    int rc;
    rc = sqlite3_open("users.db", &murk_userdb);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(murk_userdb));
        exit(EXIT_FAILURE);
    }
}


void Server::DeInit()
{
    enet_host_destroy(server);
	atexit(enet_deinitialize);
	sqlite3_close(murk_userdb);
}


void Server::Init()
{
    InitEnet();
    InitSQL();
}


void Server::ProcessEvent(ENetEvent event)
{

    //size_t len;
    Murk::User _nuser;
    Murk::Packet _p;
    Murk::Screen* _s;
    printf("event peer %p\n", event.peer);
    _p.SetPeer(event.peer);
            
    switch (event.type)
    {
        case ENET_EVENT_TYPE_CONNECT:
            printf("Client connected from %x:%u.\n",
                    event.peer->address.host, event.peer->address.port);
            
            // Create a unique guid for the user 
            char _guid[16];
            generate_new_guid(_guid);
            
            // Allocate a user and copy the guid 
            _nuser.SetID(&_guid[0]);
            _nuser.SetScreen((void*)&mainmenu); // todo: static_cast<> ? 
            activeUserMap[_guid] = _nuser;  // Assign "guid" = MurkUser
            event.peer->data = &activeUserMap[_guid];
            //_s = (Murk::Screen*)_nuser.currentScreen;
            
            enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            /* Receive event type */
            printf("[Debug] EVENT RECEIVE\n");
            
            _p.SetString((const char*)event.packet->data);
            if(_p.Validate() != 0) {
                printf("Fatal: Failed parsing json blob: %s\n. Quitting ", _p.GetString().c_str());
                exit(1);
            }
            printf("[DEBUG] PACKET: %s\n", _p.GetString().c_str());
            
            _p.ParseData();     // stores values into a map 
            ProcessPacket(_p);

            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", (char *)event.peer->data);
            
            event.peer->data = NULL;
            break;
        case ENET_EVENT_TYPE_NONE:
            break;
    }

}


void Server::ProcessPacket(Packet p)
{
    std::string _t = "type";
    std::string _d = p.GetData(_t);
    //User* _u = (User*)p.GetPeer()->data;
    //printf("%s\n", _u->GetID());

    //
    // LOGIN REQUEST 
    //
    if(_d == "LOGIN_REQ"){
        if(CheckPassword(p.GetData("user"), p.GetData("pass")) == 1)
        {
            printf("[Debug] Password check for user %s OK\n", p.GetData("user").c_str());
            // send the welcome/main menu packet 
            Murk::Packet newp(mainmenu_json);
            if(newp.Validate()!=0) printf("[Debug] Error with main menu packet header file, not sent.\n");

            ENetPacket *packet = enet_packet_create(newp.GetString().c_str(), newp.GetString().length(),
                ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(p.GetPeer(), 0, packet);
            
        }
        else { 
            printf("[Debug] Invalid login from user %s\n", p.GetData("user").c_str());
        }
    }    
    //
    // MENU SELECTION 
    //
    else if(_d == "MENUSEL"){
        User* _u = (User*)p.GetPeer()->data; // What is the User?
        // What screen is the user on?
        //Murk::Screen _s = _u.
    }
    
}

// Copies the returned PW from SQL db into a buffer
int Server::pw_callback(void *_pass, int argc, char **argv, char **azColName)
{
    for (int i = 0; i < argc; i++)
    {
        std::string _pw = "password";
        std::string azc = azColName[i];
        
        if (azc.find(_pw) != std::string::npos)
        {
            char buf[128];
            for(int j = 0; j < 128; j++) buf[j] = argv[i][j];
            
            std::string b = buf;
            
            b.copy((char*)_pass, 128, 0);
            
        }
    }
    

    return 0;
}

bool Server::CheckPassword(std::string un, std::string pw)
{
    //extern std::string scratch; 
    
    std::string command = "select password from users where user_id = '" + un + "';";
    int rc;
    char* zErrMsg;
    // function pointer
    char _pass[128] = { 0 };
    rc = sqlite3_exec(murk_userdb, command.c_str(), pw_callback, &_pass[0], &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Check PW SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return 0;
    }
    else
    {
        std::string _o = _pass;
        if (_o == "") return 0;
         // do compare now, _pass vs pw 
        std::size_t _f = pw.find(_pass);
        if(_f != std::string::npos) {
            //printf("Password match.\n");
            return 1;
        }
        //else { return 0; }

        return 0;
    }
    return 0;
}

//! Initialize ENet driver
//!
void Server::InitEnet()
{

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Error while initializing ENet.\n");
        exit(EXIT_FAILURE);
    }

    address.host = ENET_HOST_ANY;  // localhost
    address.port = 1234;
    server = enet_host_create(&address, 128, 2, 0, 0);
    if (server == NULL)
    {
        fprintf(stderr, "An error occurred trying to create server\n");
        exit(EXIT_FAILURE);
    }

    // OK!
    printf("MURK server started on localhost:%d \n", address.port);
    return;
}

}

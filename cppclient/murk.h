//! @file murk.h Primary MURK structure header!

#ifndef __MURK_H__
#define __MURK_H__

typedef unsigned char u8;
//typedef unsigned char bool;
typedef unsigned char byte;

typedef struct json_object_element_s *JSONElement;
typedef struct json_string_s *JSONString;
typedef struct json_value_s *JSONVal;
typedef struct json_object_s *JSONObject;

#define true 1
#define false 0

//! Macro to compare the next value in the json blob. 
#define JNEXT(x) (strcmp(next_name->string, x) == 0)

//! Macro to compare the incoming packet type. 
#define PACKETTYPEIS(a) (strcmp(p_type_valstr->string, #a) == 0)

typedef struct login
{
    char *userName;
    char *password;
} s_login;

enum MURK_PACKET_TYPES
{
    MP_NOPACKET = 0,
    MP_LOGIN_REQ = 1, // user > server
    MP_LOGIN_WELCOME = 2, // aka "main menu" server > user
    MP_MESSAGE_GEN = 3, // generic message 
    MP_MENU = 4, // generic menu type 
    MP_MENUSEL = 5, // response to a menu 
    MP_PCOMMAND = 6, // generic player command 
};

typedef enum packet_action //! client
{
    PA_NONE = 0,
    PA_LOGIN = 1,
    PA_MENUSELECT = 2
} PacketAction;

typedef enum process_action //! server
{
    PRA_NONE = 0,
    PRA_PROCESSLOGIN = 1,
    PRA_MENUSELECT = 2
} ProcessAction;

//! For typical room structure of generic MUDs.
typedef struct _exits { 
    bool N;
    bool S;
    bool E;
    bool W;
    bool Up;
    bool Down;
    bool NW;
    bool NE;
    bool SE;
    bool SW;
} Exits;

#define no_exits 0,0,0,0,0,0,0,0,0,0

enum ScreenType { 
    LOGIN_SCREEN = 0,
    MAIN_MENU = 1,
    SAFE_AREA = 2,
    NORMAL_AREA = 3
};

//! The standard building block of MURK. 
//! Note that this will expand by a LOT in the future.
//! @param id_no Unique identifier number of the room. 
//! @param type ScreenType, be it safe room, adventuring room, menu, etc.
//! @param exits Exits from the room, if any
//! @param description Pointer to the const char description of the room.
typedef struct _screen { 
    int id_no;
    enum ScreenType type;
    Exits exits;
    const char* description;
} Screen;


enum PlayerState { 
    STATE_OFFLINE = 0,
    STATE_MAINMENU = 1,
    STATE_IDLE = 2,
    STATE_INCOMBAT = 3,
    STATE_DEAD = 4
};


//! The user data itself. Note that all of this is prone to change. 
//! @param id peer->data to identify user vs peer. 
//! @param currentScreen the current scren the user is on. 
//! @param state the user's current state, if applicable. 
//! @param userName the username.
//! @param level the user's player level. 
//! @param miscBytes placeholder bytes for size reasons.
typedef struct _user_state {  // this is pointed to by user->data.
    char id[16]; // current GUID
    Screen* currentScreen; 
    enum PlayerState state;
    char userName[64];
    int level;
    char miscBytes[256];
} UserState;
// 512 at most?


#endif
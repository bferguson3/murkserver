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
    MP_LOGIN_REQ = 1,
    MP_LOGIN_WELCOME = 2,
    MP_MESSAGE_GEN = 3,
    MP_MENUSEL = 4,
    MP_PCOMMAND = 5,
    MP_MESSAGE_SCREEN = 6,
    MP_ROOM_INFO = 7
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
    /*bool NW;
    bool NE;
    bool SE;
    bool SW;*/
} Exits;


enum ItemType { 
    IT_UNUSABLE   = 0,
    IT_ONLYMAP    = 1,
    IT_ONLYBATTLE = 2,
    IT_EVERYWHERE = 3,
    IT_EQUIPMENT  = 4
};

#define ITEMFLAG_EQUIPPABLE_FIGHTER (1 << 0)
#define ITEMFLAG_EQUIPPABLE_RANGER  (1 << 1)
#define ITEMFLAG_EQUIPPABLE_MAGE    (1 << 2)
#define ITEMFLAG_EQUIP_BODY         (1 << 3)
#define ITEMFLAG_EQUIP_ARM          (1 << 4)
#define ITEMFLAG_EQUIP_WEAPON       (1 << 5)
#define ITEMFLAG_EQUIP_CURSE        (1 << 6)
#define ITEMFLAG_JUNK               (1 << 7)
#define ITEMFLAG_MAGICAL            (1 << 8)
#define ITEMFLAG_USE_CONSUME        (1 << 9)
#define ITEMFLAG_USEABLE_BATTLE     (1 << 10)
#define ITEMFLAG_USEABLE_MAP        (1 << 11)

#define no_exits 0,0,0,0,0,0//,0,0,0,0

enum ScreenType { 
    SCR_LOGIN = 0,
    SCR_MAIN_MENU = 1,
    SCR_SAFE_AREA = 2,
    SCR_NORMAL_AREA = 3
};

//! The standard building block of MURK. 
//! Note that this will expand by a LOT in the future.
//! @param id_no Unique identifier number of the room. 
//! @param type ScreenType, be it safe room, adventuring room, menu, etc.
//! @param exits Exits from the room, if any
//! @param description Pointer to the const char description of the room.
/*
typedef struct _screen { 
    int id_no;
    enum ScreenType type;
    Exits exits;
    const char* description;
} Screen;
*/

#define MURK_VERB_ATTACK    "ATTACK"
#define MURK_VERB_SAY       "SAY"

#define MURK_TARGET_LOCAL   "LOCAL"
#define MURK_TARGET_SELF    "SELF"

#define MURK_VERB_SOUTH     "SOUTH"
#define MURK_VERB_NORTH     "NORTH"
#define MURK_VERB_WEST      "WEST"
#define MURK_VERB_EAST      "EAST"
#define MURK_VERB_UP        "UP"
#define MURK_VERB_DOWN      "DOWN"


enum PlayerState { 
    STATE_OFFLINE = 0,
    STATE_MAINMENU = 1,
    STATE_IDLE = 2,
    STATE_INCOMBAT = 3,
    STATE_DEAD = 4,
    STATE_LOGGING_IN = 5,
    STATE_NORMAL = 6
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
    //Screen* currentScreen; 
    enum PlayerState state;
    char userName[64];
    int level;
    char miscBytes[256];
} UserState;
// 512 at most?


#define MENU_SELECT_STRING "> "

#endif
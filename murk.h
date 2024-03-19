#ifndef __MURK_H__
#define __MURK_H__

typedef unsigned char u8;
typedef unsigned char bool;
typedef unsigned char byte;

typedef struct json_object_element_s *JSONElement;
typedef struct json_string_s *JSONString;
typedef struct json_value_s *JSONVal;
typedef struct json_object_s *JSONObject;

#define true 1
#define false 0
#define JNEXT(x) (strcmp(next_name->string, x) == 0)
#define PACKETTYPEIS(a) (strcmp(p_type_valstr->string, #a) == 0)

typedef struct login
{
    char *userName;
    char *password;
} s_login;

enum MURK_PACKET_TYPES
{
    VOID = 0,
    LOGIN_REQ = 1,
    LOGIN_WELCOME = 2,
};

typedef enum packet_action
{
    PA_NONE = 0,
    PA_LOGIN = 1,
    PA_MENUSELECT = 2
} PacketAction;

typedef enum process_action
{
    PRA_NONE = 0,
    PRA_PROCESSLOGIN = 1,
    PRA_MENUSELECT = 2
} ProcessAction;



#endif
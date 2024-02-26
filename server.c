// murk server.c

#include <enet/enet.h>
#include <sqlite3.h>
#include <stdio.h>

#include "base64.h"
#include "guid.h"
#include "json.h"
#include "jsonres.h"
#include "murk.h"

ENetAddress address;
ENetHost *server;

const char *welcome_packet =
    "{\
 \"packetType\":\"LOGIN_WELCOME\",\
 \"text\":\"Welcome to \x1b[31mMUDVERSE!\x1b[0m\",\
 \"tickRate\":250\
}";

char decrypted_pass[128] = {0};
s_login user;

// fdefs
void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer);
void CreateNewUser(char *un, char *pw);
bool CheckUserPass(char *un, byte *pw);
void g_copy(char *src, char *dst, size_t len);
size_t GetBlobLength(char *table, char *row, char *key, char *val);

// local tick rate
#define TICK_RATE 250

sqlite3 *murk_userdb;
byte password_buffer[128] = {0};
size_t last_buffer_size = 0;

void clear_password_buffer()
{
    long *ptr = (long *)&password_buffer[0];
    for (int i = 0; i < (128 / sizeof(long)); i++)
    {
        *ptr = (long)0;
        ptr++;
    }
}

static int pw_callback(void *_notused, int argc, char **argv, char **azColName)
{
    clear_password_buffer();
    for (int i = 0; i < argc; i++)
    {
        // is it a password field?
        if (strcmp(azColName[i], "password") == 0)
        {
            // g_copy(argv[i], password_buffer, strlen(argv[i]));
            char *_a = (char *)&password_buffer[0];
            char *_b = &argv[i][0];
            // printf("pb %s\nargv %s\n", _a, _b);
            // printf("sz %d\n", last_buffer_size);
            for (int j = 0; j < last_buffer_size; j++) _a[j] = _b[j];
        }
    }
    // printf("%s\n", password_buffer);  // THIS IS RAW HASH.
    return 0;
}

static int bloblen_callback(void *_notused, int argc, char **argv,
                            char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if (strcmp("length(password)", azColName[i]) == 0)
        {
            // printf("password length %s\n", argv[i]);
            last_buffer_size = atoi(argv[i]);
        }
    }
    // printf("\n");
    return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

//
int main(int argc, char **argv)
{
    /// database
    int rc;
    rc = sqlite3_open("users.db", &murk_userdb);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n",
                sqlite3_errmsg(murk_userdb));
        return -1;
    }

    // initialize enet
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Error while initializing ENet.\n");
        return EXIT_FAILURE;
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
    printf("MURK server started on localhost:%d with tick rate of %d\n",
           address.port, TICK_RATE);

    ENetEvent event;
    while (1)
    {
        // infinite loop
        while (enet_host_service(server, &event, TICK_RATE) >
               0)  // is there an event?
        {
            char *mypacket;
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    /* Connection event */
                    printf("Client connected from %x:%u.\n",
                           event.peer->address.host, event.peer->address.port);

                    event.peer->data =
                        generate_new_guid();  //"Test"; // Generate user ID here

                    ENetPacket *packet = enet_packet_create(
                        welcome_packet, strlen(welcome_packet),
                        ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(event.peer, 0, packet);
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    /* Receive event type */
                    printf("[Debug] EVENT RECEIVE\n");
                    size_t len = event.packet->dataLength;
                    mypacket = (char *)malloc(len);
                    memcpy(mypacket, event.packet->data, len);
                    ProcessPacket(mypacket, len, event.peer);
                    enet_packet_destroy(event.packet);
                    free(mypacket);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    /* Disconnect event */
                    printf("%s disconnected.\n", (char *)event.peer->data);
                    // event.peer->data = NULL;
                    free(event.peer->data);
                    break;

                case ENET_EVENT_TYPE_NONE:

                    break;
            }
        }
    }

    // De-init server
    enet_host_destroy(server);
    atexit(enet_deinitialize);
    sqlite3_close(murk_userdb);

    return 0;
}

void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer)
{
    printf("[Debug] Packet length: %zu len\n", len);
    // printf("%s\n", pkt);

    //  parse json object
    JSONVal j = json_parse(pkt, len);

    // into a struct
    JSONObject object = (JSONObject)j->payload;
    JSONElement p_type = object->start;
    JSONString p_type_str = p_type->name;

    ProcessAction p_act;

    // Packet Type verification
    // error out if element 0 is not packetType (change later)
    if (strcmp(p_type_str->string, "packetType") != 0)
    {
        printf("Error: packetType not found\n");
        return;
    }

    // get the packet type
    JSONVal p_type_val = p_type->value;
    JSONString p_type_valstr = p_type_val->payload;
    // Debug print the packet type
    // printf("[Debug] packetType: %s\n", p_type_valstr->string);

    // assign last json element
    JSONElement last_j = p_type;

    // vars we might need:
    bool passwordOK = false;
    bool processLogin = false;
    int userIndex = -1;
    void (*FinalizeFunction)();
    enum MURK_PACKET_TYPES packetType;

    // continue while there are still elements in the blob
    while (last_j->next != json_null)
    {
        // get next element:
        JSONElement next_j = last_j->next;
        JSONString next_name = next_j->name;
        JSONVal next_val = next_j->value;

        //
        if (strcmp(p_type_valstr->string, "LOGIN_REQ") == 0)
        {
            // Login Request packet from user
            //
            // FinalizeFunction = finalizeLoginReq;
            //
            if (JNEXT("user"))  // username
            {
                JSONString next_str = next_val->payload;
                // TODO: protect user db during these operations
                // for (int i = 0; i < USER_DB_COUNT; i++)
                //{
                //    if (strcmp(next_str->string, user_db[i].userName) == 0)
                //    {
                /* USER LOGIN FOUND IN USER DB */
                //        printf("User found. Checking PW.\n");
                // processLogin = true;
                p_act = PRA_PROCESSLOGIN;
                user.userName =
                    (char *)next_str->string;  // user_db[i].userName;

                last_buffer_size = GetBlobLength("users", "user_id", "password",
                                                 user.userName);
                // GetBlobLength("users", "user_id", "password", "ben");
                // userIndex = i;
                // }
                //  else
                //{
                /* USERNAME NOT FOUND */
                //    printf("User not found.\n");
                // processLogin = false;
                //    enet_peer_disconnect(peer, 0);
                //}
            }
            else if (JNEXT("pass")) /* handle password field */
            {
                JSONString pw = next_val->payload;
                //Base64decode((char *)&decrypted_pass, (char *)pw->string);
                user.password = pw->string;//(char *)&decrypted_pass;
                //char decrypted[128] = {0};
                //printf("DEBUG GG:%s \n", decrypted_pass);
            }
        }
        //
        // Other packets
        //
        // else { } // other packets

        last_j = next_j;
    }

    // Final processing loop
    if (p_act == PRA_PROCESSLOGIN)
    {
        if (CheckUserPass(user.userName, (byte *)user.password))
        {
            printf("Login OK!\n");
            // LOGIN SUCCESSFUL !

            ENetPacket *packet =
                enet_packet_create(mainmenu_json, strlen(mainmenu_json),
                                   ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);

            // create a new packet that is a login welcome (make char etc)
        }
        else
        {
            printf("Invalid username or password. Ctrl+C to exit...\n");
        }
    }
    else
    {
    }

    free(j);
}

bool bytecmp(void *src, void *dst, size_t count)
{
    unsigned char *p1 = src;
    unsigned char *p2 = dst;
    for (size_t c = 0; c < count; c++)
    {
        if (*p1 != *p2) return 0;
    }
    return 1;  // OK
}

size_t GetBlobLength(char *table, char *row, char *key, char *val)
{
    char *zErrMsg = 0;
    int rc;
    char *sqlcmd_a = "select length(";
    // key: password
    char *sqlcmd_b = ") from ";
    // table: users
    char *sqlcmd_c = " where ";
    // row: user_id
    char *sqlcmd_d = " = '";
    // val: ben
    // ';

    char *sqlcmd_f =
        (char *)malloc(strlen(sqlcmd_a) + strlen(sqlcmd_b) + strlen(sqlcmd_c) +
                       strlen(sqlcmd_d) + strlen(key) + strlen(table) +
                       strlen(row) + strlen(val) + 2);
    char *_loc = sqlcmd_f;
    g_copy(sqlcmd_a, _loc, strlen(sqlcmd_a));
    _loc += strlen(sqlcmd_a);
    g_copy(key, _loc, strlen(key));
    _loc += strlen(key);
    g_copy(sqlcmd_b, _loc, strlen(sqlcmd_b));
    _loc += strlen(sqlcmd_b);
    g_copy(table, _loc, strlen(table));
    _loc += strlen(table);
    g_copy(sqlcmd_c, _loc, strlen(sqlcmd_c));
    _loc += strlen(sqlcmd_c);
    g_copy(row, _loc, strlen(row));
    _loc += strlen(row);
    g_copy(sqlcmd_d, _loc, strlen(sqlcmd_d));
    _loc += strlen(sqlcmd_d);
    g_copy(val, _loc, strlen(val));
    _loc += strlen(val);

    // char *p = &sqlcmd_f[strlen(sqlcmd_f) - 2];
    *_loc++ = '\'';
    *_loc++ = ';';

    // printf("%s\n", sqlcmd_f);

    // the callback assigns last_buffer_size
    rc = sqlite3_exec(murk_userdb, sqlcmd_f, bloblen_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Bloblen SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return 0;
    }

    return last_buffer_size;
}

// select length(password) from users where user_id = 'ben';

bool CheckUserPass(char *un, byte *pw)
{
    char *zErrMsg = 0;
    int rc;
    char *sqlcmd_a = "select password from users where user_id = '";
    // UID
    char *sqlcmd_b = "';\x00";
    char *sqlcmd_f =
        (char *)malloc(strlen(sqlcmd_a) + strlen(sqlcmd_b) + strlen(un));

    g_copy(sqlcmd_a, sqlcmd_f, strlen(sqlcmd_a));
    g_copy(un, &sqlcmd_f[0] + strlen(sqlcmd_a), strlen(un));
    g_copy(sqlcmd_b, &sqlcmd_f[0] + strlen(sqlcmd_a) + strlen(un),
           strlen(sqlcmd_b));

    rc = sqlite3_exec(murk_userdb, sqlcmd_f, pw_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Password SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return 0;
    }
    else
    {
        // now compare user.password
        printf("UP:%s\nPB:%s\n", user.password, password_buffer);
        //  and password_buffer
        if (bytecmp(user.password, password_buffer,
                    last_buffer_size))  // == 1 is OK
        {
            return 1;
        }
        // command OK
    }
    return 0;
}

void CreateNewUser(char *un, char *pw)
{
    // initialize sqlite db
    char *zErrMsg = 0;
    int rc;

    char *sqlcmd_a =
        "insert or ignore into users (user_id, password) "
        "values ('";
    // create custom sql command
    char *sqlcmd_f =
        (char *)malloc(strlen(sqlcmd_a) + strlen(pw) + strlen(un) + 8);
    int i;
    for (i = 0; i < strlen(sqlcmd_a); i++) sqlcmd_f[i] = sqlcmd_a[i];
    for (int n = 0; n < strlen(un); n++) sqlcmd_f[i++] = un[n];
    sqlcmd_f[i++] = '\'';
    sqlcmd_f[i++] = ',';
    sqlcmd_f[i++] = ' ';
    sqlcmd_f[i++] = '\'';
    for (int n = 0; n < strlen(pw); n++) sqlcmd_f[i++] = pw[n];
    sqlcmd_f[i++] = '\'';
    sqlcmd_f[i++] = ')';
    sqlcmd_f[i++] = ';';
    sqlcmd_f[i++] = '\x00';

    // printf("[Debug] SQL command: %s\n", sqlcmd_f);

    rc = sqlite3_exec(murk_userdb, sqlcmd_f, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "User record created (if did not already exist).\n");
    }
}

void g_copy(char *src, char *dst, size_t len)
{
    for (int i = 0; i < len; i++) *dst++ = *src++;
}

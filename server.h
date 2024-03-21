//! @file server.h
//! Most non-packet server-side code is here.


#include <enet/enet.h>

void g_copy(char *src, char *dst, size_t len);

//! SQL callback from getting blob length. 
static int bloblen_callback(void *_notused, int argc, char **argv,
                            char **azColName);

//! SQL callback from getting password. 
static int pw_callback(void *_notused, int argc, char **argv, char **azColName);

//! 1:1 byte compare. 
bool bytecmp(void *src, void *dst, size_t count);

//! Generic SQL callback.
static int callback(void *NotUsed, int argc, char **argv, char **azColName);

//! Normal packet processing loop for the server. 
void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer);

//! Proffers a menu to the peer and processes their selection when it comes back. 
void ProcessMenu(ENetPeer* peer, Screen* menu, char s);

//! Standard ENet event processor. 
void ProcessEvent(ENetEvent event);

//! Performs secondary processing of packet action. 
ProcessAction ProcessPacketAction(const char *pkt, size_t len, ENetPeer *peer);

//! Sends a message to a single peer. Messages have no callback or reply needed, just messages. 
void SendMessagePacket(const char* msg, ENetPeer* peer);

//! Allocates and returns a concatenated string. 
char* str_append(char* a, char* b);

//! Defines the desired tick rate of the server - how many m/s between each user's update.
#define TICK_RATE 250
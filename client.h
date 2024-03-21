//! @file client.h Client processing stuff.

#ifndef __MURKCLIENT_H__
#define __MURKCLIENT_H__

//! Creates a JSON string for logging in. 
char *ConstructLoginPacket(char *un, void *pw);

//! Creates a JSON string with a single character selection. 
char *ConstructMenuPacket(char sel);

//! The primary packet processing function. 
/*! 
    Sorts through all packet data, stores in a struct, and then executes 
    any remaining commands if necessary. 
    @param pkt The raw contents of the packet. 
    @param len The length of the packet. 
    @param peer Pointer to the peer that sent the packet. 
*/
void ProcessPacket(const char *pkt, size_t len, ENetPeer *peer);

ssize_t getpasswd(char **pw, size_t sz, int mask, FILE *fp);

//! A byte-copy. Not fast.
void g_copy(char *src, char *dst, size_t len);

//! Converts the password into a 64-character encryption. 
void *encrypt_pass(char *pass);

void assert(bool tf);

//! Gets text from a command prompt until enter is pressed.
const char *CmdPrompt(const char *prompt);

//! Standard packet processing function. 
//! Completes processing of any data loaded by ProcessPacket().
void ProcessClientPacket(ENetEvent* event);

#endif 
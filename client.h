//! @file client.h Client processing stuff.
//! The client loop is such: 
//! - ProcessEvent()
//! - ProcessPacket() (if exists)
//! - ProcessPacket_Type() fills out the user's command struct 
//! - ProcessPacket_Final() will finalize any remaining output if needed

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

//! Gets password from command line masked with *s.
size_t getpasswd(char **pw, size_t sz, int mask, FILE *fp);

//! A byte-copy. Not fast.
void g_copy(char *src, char *dst, size_t len);

//! Converts the password into a 64-character encryption. 
void *encrypt_pass(char *pass);

void assert(bool tf);

//! Gets text from a command prompt until enter is pressed.
const char *CmdPrompt(const char *prompt);

//! Standard event processing loop. 
void ProcessEvent(ENetEvent* event);

//! Assigns the p_act variable and player process struct data. 
void ProcessPacket_Type(JSONElement next_j, PacketAction* p_act, const char* pType);

//! Secondary processs loop, if needed.
//! Completes processing of any data loaded by ProcessPacket().
void ProcessPacket_Final(PacketAction p_act, ENetPeer* peer);

#endif 
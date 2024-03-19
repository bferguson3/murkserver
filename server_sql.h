#ifndef __serversqlh__
#define __serversqlh__

#include <sqlite3.h>


size_t GetBlobLength(char *table, char *row, char *key, char *val);

// select length(password) from users where user_id = 'ben';

bool CheckUserPass(char *un, byte *pw);

void CreateNewUser(char *un, char *pw);

#endif 
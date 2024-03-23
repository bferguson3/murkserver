//! @file server_sql.h 
//! @brief Server-side SQL operations.

#ifndef __serversqlh__
#define __serversqlh__

#include <sqlite3.h>


//! Get the length of a blob to/from the db.
size_t GetBlobLength(char *table, char *row, char *key, char *val);

//! Verify user password, encoded as-is 
bool CheckUserPass(char *un, byte *pw);

//! Not yet implemented 
void CreateNewUser(char *un, char *pw);

#endif 
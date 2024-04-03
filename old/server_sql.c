// server_sql.c 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "murk.h"
#include "server.h"
#include "server_sql.h"

extern sqlite3 *murk_userdb;
size_t _last_buffer_size = 0;
extern s_login user;
extern byte password_buffer[128];

size_t GetBlobLength(char *table, char *row, char *key, char *val)
{
    _last_buffer_size = 0;
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

    size_t _len = strlen(sqlcmd_a) + strlen(sqlcmd_b) + strlen(sqlcmd_c) +
                       strlen(sqlcmd_d) + strlen(key) + strlen(table) +
                       strlen(row) + strlen(val)+3; // huh?
    char *sqlcmd_f =
        (char *)malloc(_len);
    //for(int i = 0; i < _len; i++) sqlcmd_f[i] = '\00';
    
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
    *_loc++ = '\00';
    //*_loc++ = 13;
    
    //printf("%s\n", sqlcmd_f);

    // the callback assigns _last_buffer_size
    rc = sqlite3_exec(murk_userdb, sqlcmd_f, bloblen_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Bloblen SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return 0;
    }

    printf("%d\n", _last_buffer_size);

    return _last_buffer_size;
}

// select length(password) from users where user_id = 'ben';

bool CheckUserPass(char *un, byte *pw)
{
    char *zErrMsg = 0;
    int rc;
    char *sqlcmd_a = "select password from users where user_id = '";
    // UID
    char *sqlcmd_b = "';";
    size_t _len = strlen(sqlcmd_a) + strlen(sqlcmd_b) + strlen(un);
    char *sqlcmd_f =
        (char *)malloc(_len);

    g_copy(sqlcmd_a, sqlcmd_f, strlen(sqlcmd_a));
    g_copy(un, &sqlcmd_f[0] + strlen(sqlcmd_a), strlen(un));
    g_copy(sqlcmd_b, &sqlcmd_f[0] + strlen(sqlcmd_a) + strlen(un),
           strlen(sqlcmd_b));
    sqlcmd_f[_len] = 0;

    printf("%s\n", sqlcmd_f);

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
        //printf("UP:%s\nPB:%s\n", user.password, password_buffer);
        
        //  and password_buffer
        if (bytecmp(user.password, password_buffer,
                    _last_buffer_size))  // == 1 is OK
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
            // printf("sz %d\n", _last_buffer_size);
            for (int j = 0; j < _last_buffer_size; j++) _a[j] = _b[j];
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
            _last_buffer_size = atoi(argv[i]);
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

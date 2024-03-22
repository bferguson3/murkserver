//! @file getpasswd.h
//! @brief get password from console. 


#define MAXPW 64
/* read a string from fp into pw masking keypress with mask char.
getpasswd will read upto sz - 1 chars into pw, null-terminating
the resulting string. On success, the number of characters in
pw are returned, -1 otherwise.
*/
ssize_t getpasswd(char **pw, size_t sz, int mask, FILE *fp);
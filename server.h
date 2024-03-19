void g_copy(char *src, char *dst, size_t len);
static int bloblen_callback(void *_notused, int argc, char **argv,
                            char **azColName);
static int pw_callback(void *_notused, int argc, char **argv, char **azColName);
bool bytecmp(void *src, void *dst, size_t count);
static int callback(void *NotUsed, int argc, char **argv, char **azColName);

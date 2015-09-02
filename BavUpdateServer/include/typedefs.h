typedef void VOID;
typedef char CHAR;
typedef const char *PCSTR;
typedef unsigned char UCHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef int INT, LONG, BOOL;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;


#define ERR(...) {printf(__VA_ARGS__);}
#define DEBUG_PRINTF

#ifdef DEBUG_PRINTF
#define DBG(...) {printf(__VA_ARGS__);}
#else
#define DBG(...)
#endif

#define BAV_OK 0
#define BAV_ERROR -1

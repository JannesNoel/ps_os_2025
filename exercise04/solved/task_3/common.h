#define FIFO_BASE_NAME "/tmp/fifo_csbb9456_"
#define FIFO_BASE_NAME_LEN strlen(FIFO_BASE_NAME)

// on the zid server, this isn't set in the limits.h header so this is a fallback which uses the save value as my mac (hope this works)
#ifndef PIPE_BUF
    #define PIPE_BUF 512 
#endif
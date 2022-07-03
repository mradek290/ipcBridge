
#ifndef IPC_BRIDGE_IMPL__
#define IPC_BRIDGE_IMPL__

#include "ipcbridge.h"

unsigned ipcb__MakePipeName( const char* desiredname, unsigned desiredname_sz, char* buffer, unsigned buffer_sz ){

    const char* prefix = "\\\\.\\pipe\\";
    const unsigned prefix_sz = strlen(prefix); //optimizer will eat strlen call

    if( !desiredname || !desiredname_sz || !buffer || !buffer_sz )
        return 0;

    unsigned len = prefix_sz + desiredname_sz;
    if( buffer_sz < len )
        return 0;

    memcpy( buffer, prefix, prefix_sz );
    memcpy( buffer + prefix_sz, desiredname, desiredname_sz );
    buffer[len] = 0;

    return len;
}

#endif

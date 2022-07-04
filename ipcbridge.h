
#ifndef IPC_BRIDGE_HEADER__
#define IPC_BRIDGE_HEADER__

#include <windows.h>

#define ipcb__ServerDefaultBufferSize (1<<10)
#define ipcb__ServerDefaultTimeout 200

typedef enum{
    ipcberr_NoError = 0,
    ipcberr_ServerNameInvalid,
    ipcberr_ServerCreationFailed,
    ipcberr_ServerAwaitConnectionFailure,
    ipcberr_ServerCannotIdentifyClient,
    ipcberr_BridgeInitializationFailure,
    ipcberr_UnableToLoadClientIdentity,
    ipcberr_BridgeSharingFailure,
    ipcberr_CannotCreateSynchronization,
    ipcberr_CannotShareSynchronization,
    ipcberr_ServerCannotOpenSharedMemory,
    ipcberr_ServerCannotRespondToClient,
    ipcberr_ClientConnectionAwaitFailure,
    ipcberr_ClientConnectFailure,
    ipcberr_ClientCannotIdentifyToServer,
    ipcberr_InvalidBridge,
    ipcberr_ClientCannotOpenSharedMemory,
    ipcberr_FailedToAwaitSignal,
    ipcberr_FailedToSendSignal
} ipcbError;

typedef enum{
    ipcbside_Server = (1 << 0),
    ipcbside_Client = (1 << 1)
} ipcbSide;

typedef void ipcbServer;

typedef struct ipcb__ControlInstance{
    _Bool isServerActive;
    _Bool isClientActive;
} ipcbControlInstance;

typedef struct ipcb__HandleGroup{
    void* SharedMemory;
    void* ServerNotification;
    void* ClientNotification;
    void* MemoryAddress;
    ipcbControlInstance* Control;
} ipcbHandleGroup;

typedef struct ipcb__Bridge{
    ipcbHandleGroup Server;
    ipcbHandleGroup Client;
    unsigned SharedMemorySize;
    ipcbError Error;
} ipcbBridge;

#define ipcb__ControlInstanceOffset (sizeof(void*) * (1 + sizeof(ipcbControlInstance)/sizeof(void*)))

ipcbServer* ipcbInitServer( const char*, ipcbError* );
ipcbBridge* ipcbAwaitConnection( ipcbServer*, ipcbError* );
void        ipcbShutdownServer( ipcbServer*, ipcbError* );
ipcbBridge* ipcbConnectServer( const char*, unsigned long long, ipcbError* );
void ipcbCloseBridge( ipcbBridge*, ipcbSide );

unsigned ipcbWriteToClient( ipcbBridge*, unsigned, const void*, unsigned );
unsigned ipcbWriteToserver( ipcbBridge*, unsigned, const void*, unsigned );

void ipcbAwaitServer( ipcbBridge*, ipcbError* );
void ipcbAwaitClient( ipcbBridge*, ipcbError* );

void ipcbSignalServer( ipcbBridge*, ipcbError* );
void ipcbSignalClient( ipcbBridge*, ipcbError* );

unsigned ipcbReadFromClient( ipcbBridge*, unsigned, void*, unsigned );
unsigned ipcbReadFromServer( ipcbBridge*, unsigned, void*, unsigned );

/*
 * TODO
 * implement server shutdown
 * implement cleanup for connect server failing to open shared region
 * implement close bridge
 * resolve error codes to messages
 * implement reading from shared mem
*/

#include "ipcbridge.c"

#endif

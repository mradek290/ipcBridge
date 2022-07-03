
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
    ipcberr_ServerCannotRespondToClient
} ipcbError;

typedef enum{
    ipcbside_Server = (1 << 0),
    ipcbside_Client = (1 << 1)
} ipcbSide;

typedef void ipcbServer;

typedef struct ipcb__HandleGroup{
    void* SharedMemory;
    void* ServerNotification;
    void* ClientNotification;
    void* MemoryAddress;
} ipcbHandleGroup;

typedef struct ipcb__Bridge{
    ipcbHandleGroup Server;
    ipcbHandleGroup Client;
    unsigned SharedMemorySize;
    ipcbError Error;
} ipcbBridge;

#include "ipcbridge.c"

ipcbServer* ipcbInitServer( const char*, ipcbError* );
ipcbBridge* ipcbAwaitConnection( ipcbServer*, ipcbError* );
void        ipcbShutdownServer( ipcbServer*, ipcbError* );
ipcbBridge* ipcbConnectServer( const char*, ipcbError* );

#endif

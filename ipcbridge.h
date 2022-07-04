
#ifndef IPC_BRIDGE_HEADER__
#define IPC_BRIDGE_HEADER__

#ifdef IPC_BRIDGE_DLL_EXPORT
#define IPC_BRIDGE_DECLSPEC __declspec(dllexport)
#else
#define IPC_BRIDGE_DECLSPEC
#endif

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

IPC_BRIDGE_DECLSPEC ipcbServer* ipcbInitServer( const char*, ipcbError* );
IPC_BRIDGE_DECLSPEC ipcbBridge* ipcbAwaitConnection( ipcbServer*, ipcbError* );
IPC_BRIDGE_DECLSPEC void        ipcbShutdownServer( ipcbServer* );
IPC_BRIDGE_DECLSPEC ipcbBridge* ipcbConnectServer( const char*, unsigned long long, ipcbError* );
IPC_BRIDGE_DECLSPEC void ipcbCloseBridge( ipcbBridge*, ipcbSide );

IPC_BRIDGE_DECLSPEC unsigned ipcbWriteToClient( ipcbBridge*, unsigned, const void*, unsigned );
IPC_BRIDGE_DECLSPEC unsigned ipcbWriteToServer( ipcbBridge*, unsigned, const void*, unsigned );

IPC_BRIDGE_DECLSPEC void ipcbAwaitServer( ipcbBridge*, ipcbError* );
IPC_BRIDGE_DECLSPEC void ipcbAwaitClient( ipcbBridge*, ipcbError* );

IPC_BRIDGE_DECLSPEC void ipcbSignalServer( ipcbBridge*, ipcbError* );
IPC_BRIDGE_DECLSPEC void ipcbSignalClient( ipcbBridge*, ipcbError* );

IPC_BRIDGE_DECLSPEC unsigned ipcbReadFromClient( ipcbBridge* bridge, unsigned offset, unsigned toread, void* buffer, unsigned buffer_sz );
IPC_BRIDGE_DECLSPEC unsigned ipcbReadFromServer( ipcbBridge* bridge, unsigned offset, unsigned toread, void* buffer, unsigned buffer_sz );

IPC_BRIDGE_DECLSPEC _Bool ipcbIsServerConnectionOpen( ipcbBridge* );
IPC_BRIDGE_DECLSPEC _Bool ipcbIsClientConnectionOpen( ipcbBridge* );

IPC_BRIDGE_DECLSPEC const char* ipcbResolveErrorCode( ipcbError );

//------------------------------------------

#ifdef IPC_BRIDGE_DLL_EXPORT

IPC_BRIDGE_DECLSPEC unsigned ipcbGetSharedMemorySize( ipcbBridge* );

#endif

#include "ipcbridge.c"

#endif

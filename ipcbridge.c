
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

ipcbServer* ipcbInitServer( const char* name, ipcbError* e ){

    ipcbError dummyerror;
    if( !e ) e = &dummyerror;

    const unsigned namebuffer_sz = 200;
    char namebuffer[namebuffer_sz];
    unsigned pipename_sz = ipcb__MakePipeName(
        name, strlen(name),
        namebuffer, namebuffer_sz
    );

    if( pipename_sz == 0 ){
        *e = ipcberr_ServerNameInvalid;
        return 0;
    }

    ipcbServer* server = CreateNamedPipe(
        namebuffer,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_ACCEPT_REMOTE_CLIENTS,
        1 /*only one instance*/,
        ipcb__ServerDefaultBufferSize /*out buffer*/,
        ipcb__ServerDefaultBufferSize /*in buffer*/,
        ipcb__ServerDefaultTimeout,
        0 /*default security*/
    );

    if( server == INVALID_HANDLE_VALUE ){
        *e = ipcberr_ServerCreationFailed;
        return 0;
    }

    *e = ipcberr_NoError;
    return server;
}

HANDLE ipcb__CreateSharedEvent( HANDLE sharer, HANDLE* dupetarget, ipcbError* e ){

    HANDLE evt = CreateEvent(
        0 /*default security*/,
        0 /*automatic reset*/,
        0 /*start non-signaled*/,
        0 /*anonymus*/
    );

    if( !evt ){
        *e = ipcberr_CannotCreateSynchronization;
        return 0;
    }

    _Bool dupesuccess = DuplicateHandle(
        GetCurrentProcess(),
        evt,
        sharer,
        dupetarget,
        EVENT_ALL_ACCESS,
        0 /*do not inherit*/,
        0 /*default options*/
    );

    if( !dupesuccess ){
        CloseHandle(evt);
        *e = ipcberr_CannotCreateSynchronization;
        return 0;
    }

    *e = ipcberr_NoError;
    return evt;
}

void CloseHandleSecure( HANDLE h ){
    if( h && h != INVALID_HANDLE_VALUE )
        CloseHandle(h);
}

void ipcb__DeinitBridge( ipcbBridge* bridge ){
    CloseHandleSecure(bridge->Server.SharedMemory);
    CloseHandleSecure(bridge->Client.SharedMemory);
    CloseHandleSecure(bridge->Server.ServerNotification);
    CloseHandleSecure(bridge->Client.ServerNotification);
    CloseHandleSecure(bridge->Server.ClientNotification);
    CloseHandleSecure(bridge->Client.ClientNotification);
}

ipcbBridge* ipcb__InitBridge( unsigned long long clientid, unsigned long long buffer_sz, ipcbError* e ){

    ipcbBridge bridge;
    LARGE_INTEGER li_size;
    li_size.QuadPart = buffer_sz;
    _Bool duped;

    SecureZeroMemory( &bridge, sizeof(bridge) );
    bridge.SharedMemorySize = li_size.LowPart;

    bridge.Server.SharedMemory = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        0 /*default security*/,
        PAGE_READWRITE,
        li_size.HighPart, li_size.LowPart,
        0 /*anonymus*/
    );

    if( !bridge.Server.SharedMemory ){
        *e = ipcberr_BridgeInitializationFailure;
        return 0;
    }

    HANDLE clientproc = OpenProcess(
        PROCESS_DUP_HANDLE,
        0 /*do not inherit*/,
        (DWORD) clientid
    );

    if( !clientproc ){
        ipcb__DeinitBridge(&bridge);
        *e = ipcberr_UnableToLoadClientIdentity;
        return 0;
    }

    duped = DuplicateHandle(
        GetCurrentProcess(),
        bridge.Server.SharedMemory,
        clientproc,
        &(bridge.Client.SharedMemory),
        FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_COPY,
        0 /*do not inherit*/,
        0 /*default options*/
    );

    if( !duped ){
        ipcb__DeinitBridge(&bridge);
        CloseHandle(clientproc);
        *e = ipcberr_BridgeSharingFailure;
        return 0;
    }

    bridge.Server.ServerNotification = ipcb__CreateSharedEvent(
        clientproc,
        &(bridge.Client.ServerNotification),
        e
    );

    if( *e != ipcberr_NoError ){
        CloseHandle(clientproc);
        ipcb__DeinitBridge(&bridge);
        return 0;
    }

    bridge.Server.ClientNotification = ipcb__CreateSharedEvent(
        clientproc,
        &(bridge.Client.ClientNotification),
        e
    );

    if( *e != ipcberr_NoError ){
        CloseHandle(clientproc);
        ipcb__DeinitBridge(&bridge);
        return 0;
    }

    _Bool* mapping = (_Bool*) MapViewOfFile(
        bridge.Server.SharedMemory,
        FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_COPY,
        0, 0, /*no memory offset*/
        li_size.LowPart
    );

    if( !mapping ){
        CloseHandle(clientproc);
        ipcb__DeinitBridge(&bridge);
        *e = ipcberr_ServerCannotOpenSharedMemory;
        return 0;
    }

    bridge.Server.Control = (ipcbControlInstance*) mapping;
    bridge.Server.MemoryAddress = mapping + ipcb__ControlInstanceOffset;
    bridge.Server.Control->isServerActive = 1;

    ipcbBridge* result = (ipcbBridge*) malloc(sizeof(ipcbBridge));
    memcpy( result, &bridge, sizeof(ipcbBridge) );
    *e = ipcberr_NoError;

    return result;
}

void ipcbCloseBridge( ipcbBridge* bridge, ipcbSide side ){

    if( side & ipcbside_Client ){

        bridge->Client.Control->isClientActive = 0;

        UnmapViewOfFile(bridge->Client.Control);
        CloseHandle(bridge->Client.MemoryAddress);
        CloseHandle(bridge->Client.ServerNotification);
        CloseHandle(bridge->Client.ClientNotification);
    }

    if( side & ipcbside_Server ){

        bridge->Server.Control->isServerActive = 0;

        UnmapViewOfFile(bridge->Server.Control);
        CloseHandle(bridge->Server.MemoryAddress);
        CloseHandle(bridge->Server.ServerNotification);
        CloseHandle(bridge->Server.ClientNotification);
    }

    free(bridge);
}

ipcbBridge* ipcbAwaitConnection( ipcbServer* server, ipcbError* e ){

    ipcbError dummyerror;
    if( !e ) e = &dummyerror;

    _Bool waitsuccess = ConnectNamedPipe( server, 0 );
    if( !waitsuccess ){
        *e = ipcberr_ServerAwaitConnectionFailure;
        return 0;
    }

    unsigned long bytesread;
    unsigned long long clientinfo[2];
    _Bool readsuccess = ReadFile(
        server,
        clientinfo, sizeof(clientinfo),
        &bytesread, 0
    );

    if( !readsuccess || bytesread != sizeof(clientinfo) ){
        *e = ipcberr_ServerCannotIdentifyClient;
        DisconnectNamedPipe(server);
        return 0;
    }

    DWORD byteswritten;
    ipcbBridge* bridge = ipcb__InitBridge(clientinfo[0], clientinfo[1], e);
    if( *e != ipcberr_NoError ){

        ipcbBridge dummy;
        dummy.Error = *e;

        WriteFile(
            server,
            &dummy, sizeof(dummy),
            &byteswritten, 0
        );

        DisconnectNamedPipe(server);
        return 0;

    }else{

        _Bool writesuccess = WriteFile(
            server,
            bridge, sizeof(ipcbBridge),
            &byteswritten, 0
        );

        if( !writesuccess || byteswritten != sizeof(ipcbBridge) ){

            ipcbCloseBridge(
                bridge,
                ipcbside_Client | ipcbside_Server
            );

            *e = ipcberr_ServerCannotRespondToClient;
            DisconnectNamedPipe(server);
            return 0;
        }

        *e = ipcberr_NoError;
        return bridge;
    }
}

ipcbBridge* ipcbConnectServer( const char* servername, unsigned long long buffer_sz, ipcbError* e ){

    ipcbError dummyerror;
    if( !e ) e = &dummyerror;

    const unsigned namebuffer_sz = 200;
    char namebuffer[namebuffer_sz];
    unsigned name_sz = ipcb__MakePipeName(
        servername, strlen(servername),
        namebuffer, namebuffer_sz
    );

    if( name_sz == 0 ){
        *e = ipcberr_ServerNameInvalid;
        return 0;
    }

    _Bool waitsuccess = WaitNamedPipe(namebuffer, NMPWAIT_WAIT_FOREVER );
    if( !waitsuccess ){
        *e = ipcberr_ClientConnectionAwaitFailure;
        return 0;
    }

    HANDLE pipe = CreateFile(
        namebuffer,
        GENERIC_READ | GENERIC_WRITE,
        0 /*no sharing*/,
        0 /*default security*/,
        OPEN_EXISTING,
        0 /*default flags*/,
        0 /*no template*/
    );

    if( pipe == INVALID_HANDLE_VALUE ){
        *e = ipcberr_ClientConnectFailure;
        return 0;
    }

    unsigned long long clientinfo[] = {
        GetCurrentProcessId(), buffer_sz
    };

    DWORD byteswritten;
    _Bool writesuccess = WriteFile(
        pipe,
        clientinfo, sizeof(clientinfo),
        &byteswritten, 0
    );

    if( !writesuccess || byteswritten != sizeof(clientinfo) ){
        CloseHandle(pipe);
        *e = ipcberr_ClientCannotIdentifyToServer;
        return 0;
    }

    ipcbBridge bridge;
    DWORD bytesread;
    bridge.Error = ~0;

    _Bool readsuccess = ReadFile(
        pipe,
        &bridge, sizeof(ipcbBridge),
        &bytesread, 0
    );

    if( !readsuccess || bytesread != sizeof(ipcbBridge) ){
        CloseHandle(pipe);
        *e = ipcberr_ServerCannotRespondToClient;
        return 0;
    }

    if( bridge.Error != ipcberr_NoError ){
        CloseHandle(pipe);
        *e = ipcberr_InvalidBridge;
        return 0;
    }

    _Bool* mapping = (_Bool*) MapViewOfFile(
        bridge.Client.SharedMemory,
        FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_COPY,
        0, 0, /*no memory offset*/
        (DWORD) bridge.SharedMemorySize
    );

    if( !mapping ){
        
        CloseHandle(bridge.Client.SharedMemory);
        CloseHandle(bridge.Client.ServerNotification);
        CloseHandle(bridge.Client.ClientNotification);

        *e = ipcberr_ClientCannotOpenSharedMemory;
        CloseHandle(pipe);
        return 0;
    }

    bridge.Client.Control = (ipcbControlInstance*) mapping;
    bridge.Client.MemoryAddress = mapping + ipcb__ControlInstanceOffset;
    bridge.Client.Control->isClientActive = 1;

    ipcbBridge* result = (ipcbBridge*) malloc(sizeof(ipcbBridge));
    memcpy( result, &bridge, sizeof(ipcbBridge) );
    *e = ipcberr_NoError;

    CloseHandle(pipe);
    return result;
}

unsigned ipcb__WriteToSharedBuffer( _Bool* base, unsigned limit, unsigned offset, const void* buffer, unsigned buffer_sz ){

    _Bool* upperbound = base + limit;
    _Bool* targetadr = base + offset;

    if( upperbound < targetadr )
        return 0;

    if( upperbound < targetadr + buffer_sz ){
        buffer_sz = upperbound - targetadr;
    }

    memcpy( targetadr, buffer, buffer_sz );
    return buffer_sz;
}

unsigned ipcbWriteToClient( ipcbBridge* bridge, unsigned offset, const void* buffer, unsigned buffer_sz ){
    return ipcb__WriteToSharedBuffer(
        (_Bool*) bridge->Server.MemoryAddress,
        bridge->SharedMemorySize,
        offset, buffer,buffer_sz
    );
}

unsigned ipcbWriteToServer( ipcbBridge* bridge, unsigned offset, const void* buffer, unsigned buffer_sz ){
    return ipcb__WriteToSharedBuffer(
        (_Bool*) bridge->Client.MemoryAddress,
        bridge->SharedMemorySize,
        offset, buffer,buffer_sz
    );
}

void ipcb__AwaitSignal( void* signal, ipcbError* e ){

    ipcbError dummyerror;
    if( !e ) e = &dummyerror;

    DWORD status = WaitForSingleObject(signal, INFINITE);
    if( status == WAIT_OBJECT_0 ){
        *e = ipcberr_NoError;
    }else{
        *e = ipcberr_FailedToAwaitSignal;
    }
}

void ipcbAwaitServer( ipcbBridge* bridge, ipcbError* e ){
    ipcb__AwaitSignal( bridge->Client.ServerNotification, e );
}

void ipcbAwaitClient( ipcbBridge* bridge, ipcbError* e ){
    ipcb__AwaitSignal( bridge->Server.ClientNotification, e );
}

void ipcb__SendSignal( void* signal, ipcbError* e ){

    ipcbError dummyerror;
    if( !e ) e = &dummyerror;

    if( SetEvent(signal) ){
        *e = ipcberr_NoError;
    }else{
        *e = ipcberr_FailedToSendSignal;
    }
}

_Bool ipcbIsServerConnectionOpen( ipcbBridge* bridge ){
    return bridge->Client.Control->isServerActive;
}

_Bool ipcbIsClientConnectionOpen( ipcbBridge* bridge ){
    return bridge->Server.Control->isClientActive;
}

void ipcbSignalServer( ipcbBridge* bridge, ipcbError* e ){
    ipcb__SendSignal( bridge->Client.ClientNotification, e );
}

void ipcbSignalClient( ipcbBridge* bridge, ipcbError* e ){
    ipcb__SendSignal( bridge->Server.ServerNotification, e );
}

void ipcbShutdownServer( ipcbServer* server ){
    DisconnectNamedPipe(server);
    CloseHandle(server);
}

const char* ipcbResolveErrorCode( ipcbError e ){

    switch( e ){

        case ipcberr_NoError :
            return "No Error";

        case ipcberr_ServerNameInvalid :
            return "Invalid Servername";

        case ipcberr_ServerCreationFailed :
            return "Failed to instantiate Server";

        case ipcberr_ServerAwaitConnectionFailure :
            return "Server failed to await an incoming connection. Server intance may be invalid now.";

        case ipcberr_ServerCannotIdentifyClient :
            return "Server failed to identify client process. Server likely has insufficient privilege";

        case ipcberr_BridgeInitializationFailure :
            return "Unable to allocate a shared memory segment";

        case ipcberr_UnableToLoadClientIdentity :
            return "Server cannot obtain client identity. Server likely has insufficient privilege";

        case ipcberr_BridgeSharingFailure :
            return "Unable to share memory segment with client";

        case ipcberr_CannotCreateSynchronization :
            return "Server failed to created a synchronization primitive";

        case ipcberr_CannotShareSynchronization :
            return "Server failed to share a synchronization primitive with the client";

        case ipcberr_ServerCannotOpenSharedMemory :
            return "Server failed to open the shared memory segment";

        case ipcberr_ServerCannotRespondToClient :
            return "Unabel to submit bridge handles to client";

        case ipcberr_ClientConnectionAwaitFailure :
        case ipcberr_ClientConnectFailure :
            return "Unable to establish connection to Server. Server might not be available";

        case ipcberr_ClientCannotIdentifyToServer :
            return "Client failed to identify to server";

        case ipcberr_InvalidBridge :
            return "Server submitted an invalid bridge";

        case ipcberr_ClientCannotOpenSharedMemory :
            return "Client failed to open the shared memory segment";

        case ipcberr_FailedToAwaitSignal :
            return "Failed to await signal";

        case ipcberr_FailedToSendSignal :
            return "Failed to send signal";

        default : return "Error code not recognized";
    }
}

unsigned ipcb__ReadFromSharedBuffer( _Bool* base, unsigned limit, unsigned offset, unsigned toread, void* buffer, unsigned buffer_sz ){

    if( !buffer || !buffer_sz )
        return 0;

    if( buffer_sz < toread ){
        toread = buffer_sz;
    }
    _Bool* upperbound = base + limit;
    _Bool* targetadr  = base + offset;

    if( upperbound < targetadr )
        return 0;

    if( upperbound < targetadr + toread ){
        toread = upperbound - targetadr;
    }

    memcpy( buffer, targetadr, toread );
    return toread;
}

unsigned ipcbReadFromClient( ipcbBridge* bridge, unsigned offset, unsigned toread, void* buffer, unsigned buffer_sz ){
    return ipcb__ReadFromSharedBuffer(
        (_Bool*) bridge->Server.MemoryAddress,
        bridge->SharedMemorySize,
        offset, toread, buffer, buffer_sz
    );
}

unsigned ipcbReadFromServer( ipcbBridge* bridge, unsigned offset, unsigned toread, void* buffer, unsigned buffer_sz ){
    return ipcb__ReadFromSharedBuffer(
        (_Bool*) bridge->Client.MemoryAddress,
        bridge->SharedMemorySize,
        offset, toread, buffer, buffer_sz
    );
}

//------------------------------------------------------------

#ifdef IPC_BRIDGE_DLL_EXPORT

unsigned ipcbGetSharedMemorySize( ipcbBridge* bridge ){
    return bridge->SharedMemorySize;
}

#endif

#endif

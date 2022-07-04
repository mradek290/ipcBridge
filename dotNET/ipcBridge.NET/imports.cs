using System;
using System.Runtime.InteropServices;

namespace ipcBridge.NET
{
    public enum ErrorCode
    {
        NoError = 0,
        ServerNameInvalid,
        ServerCreationFailed,
        ServerAwaitConnectionFailure,
        ServerCannotIdentifyClient,
        BridgeInitializationFailure,
        UnableToLoadClientIdentity,
        BridgeSharingFailure,
        CannotCreateSynchronization,
        CannotShareSynchronization,
        ServerCannotOpenSharedMemory,
        ServerCannotRespondToClient,
        ClientConnectionAwaitFailure,
        ClientConnectFailure,
        ClientCannotIdentifyToServer,
        InvalidBridge,
        ClientCannotOpenSharedMemory,
        FailedToAwaitSignal,
        FailedToSendSignal
    }
    
    internal enum Side
    {
        Server = (1 << 0),
        Client = (1 << 1)
    }

    internal class Imports
    {
        const string NativeBinary = @"ipcbridge.dll";

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern void* ipcbInitServer(string servername, UInt32* error);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern void* ipcbAwaitConnection(void* server, UInt32* error);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern void ipcbShutdownServer(void* server);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern void* ipcbConnectServer(string servername, UInt64 sharedbuffersize, UInt32* error);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern void ipcbCloseBridge(void* bridge, Side side);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern void ipcbAwaitServer(void* bridge, UInt32* error);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern void ipcbAwaitClient(void* bridge, UInt32* error);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern void ipcbSignalServer(void* bridge, UInt32* error);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern void ipcbSignalClient(void* bridge, UInt32* error);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern bool ipcbIsServerConnectionOpen(void* bridge );

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern bool ipcbIsClientConnectionOpen(void* bridge );

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static unsafe extern UInt32 ipcbGetSharedMemorySize(void* bridge);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, EntryPoint = "ipcbWriteToClient")]
        private static unsafe extern UInt32 ipcbWriteToClient__(void* bridge, UInt32 offset, void* buffer, UInt32 buffersize);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, EntryPoint = "ipcbWriteToServer")]
        private static unsafe extern UInt32 ipcbWriteToServer__(void* bridge, UInt32 offset, void* buffer, UInt32 buffersize);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, EntryPoint = "ipcbReadFromClient")]
        private static unsafe extern UInt32 ipcbReadFromClient__(void* bridge, UInt32 offset, UInt32 toread, void* buffer, UInt32 buffersize);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, EntryPoint = "ipcbReadFromServer")]
        private static unsafe extern UInt32 ipcbReadFromServer__(void* bridge, UInt32 offset, UInt32 toread, void* buffer, UInt32 buffersize);

        [DllImport(dllName: NativeBinary, CallingConvention = CallingConvention.Cdecl, EntryPoint = "ipcbResolveErrorCode")]
        private static unsafe extern IntPtr ipcbResolveErrorCode__(UInt32 x);

        //--------------------------------------------------------------------------------------------------------------

        public static string ipcbResolveErrorCode( UInt32 error)
        {
            unsafe
            {
                var ptr = ipcbResolveErrorCode__(error);
                return Marshal.PtrToStringAnsi(ptr);
            }
        }

        public static unsafe UInt32 ipcbWriteToServer(void* bridge, UInt32 offset, byte[] buffer)
        {
            fixed (void* fixture = buffer)
            {
                return ipcbWriteToServer__(bridge, offset, fixture, (UInt32)buffer.Length);
            }
        }

        public static unsafe UInt32 ipcbWriteToClient(void* bridge, UInt32 offset, byte[] buffer)
        {
            fixed (void* fixture = buffer)
            {
                return ipcbWriteToClient__(bridge, offset, fixture, (UInt32)buffer.Length);
            }
        }

        public static unsafe UInt32 ipcbReadFromServer(void* bridge, UInt32 offset, UInt32 toread, byte[] buffer)
        {
            fixed (void* fixture = buffer)
            {
                return ipcbReadFromServer__(bridge, offset, toread, fixture, (UInt32)buffer.Length);
            }
        }

        public static unsafe UInt32 ipcbReadFromClient(void* bridge, UInt32 offset, UInt32 toread, byte[] buffer)
        {
            fixed (void* fixture = buffer)
            {
                return ipcbReadFromClient__(bridge, offset, toread, fixture, (UInt32)buffer.Length);
            }
        }

    } //End of class

} //End of namespace

using System;

namespace ipcBridge.NET
{
    public class BridgeServer : IDisposable
    {
        public const UInt32 DefaultBuffersize = (1 << 20);
        
        internal unsafe void* serverhandle;
        
        public void Dispose()
        {
            unsafe
            {
                Imports.ipcbShutdownServer(serverhandle);
            }
        }

        public BridgeServer( string servername)
        {
            unsafe
            {
                UInt32 e = 0;
                serverhandle = Imports.ipcbInitServer(servername, &e);
                if( e != 0 ) //error occured
                {
                    throw new Exception(Imports.ipcbResolveErrorCode(e));
                }
            }
        }

        public IBridge AwaitConnection()
        {
            unsafe
            {
                UInt32 e = 0;
                void* handle = Imports.ipcbAwaitConnection(serverhandle, &e);
                if( e == 0)
                {
                    return new ServerSideBridge(handle);
                }
                else
                {
                    throw new Exception(Imports.ipcbResolveErrorCode(e));
                }
            }
            
        }

        public static IBridge Connect( string servername, UInt32 buffersize )
        {
            unsafe
            {
                UInt32 e = 0;
                void* handle = Imports.ipcbConnectServer(servername, buffersize, &e);
                if( e == 0)
                {
                    return new ClientSideBridge(handle);
                }
                else
                {
                    throw new Exception(Imports.ipcbResolveErrorCode(e));
                }
            }
        }

        public static IBridge Connect(string servername) => Connect(servername, DefaultBuffersize);

    } //End of class

} //End of namespace

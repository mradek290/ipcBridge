using System;

namespace ipcBridge.NET
{
    public class BridgeServer : IDisposable
    {
        public const UInt32 DefaultBuffersize = (1 << 20);
        
        internal unsafe void* serverhandle;
        private string servername;
        
        public void Dispose()
        {
            unsafe
            {
                Imports.ipcbShutdownServer(serverhandle);
            }
        }

        public BridgeServer( string name)
        {
            unsafe
            {
                UInt32 e = 0;
                serverhandle = Imports.ipcbInitServer(name, &e);
                if( e != 0 ) //error occured
                {
                    throw new Exception(Imports.ipcbResolveErrorCode(e));
                }

                servername = name;
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
                    return new ServerSideBridge(handle,servername);
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
                    return new ClientSideBridge(handle,servername);
                }
                else
                {
                    throw new Exception(Imports.ipcbResolveErrorCode(e));
                }
            }
        }

        public static IBridge Connect(string servername) => Connect(servername, DefaultBuffersize);

        public string Name
        {
            get => servername;
        }


    } //End of class

} //End of namespace

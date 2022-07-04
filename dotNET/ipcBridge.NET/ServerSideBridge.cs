using System;

namespace ipcBridge.NET
{
    internal class ServerSideBridge : IBridge
    {
        internal unsafe void* bridgehandle;
        private string sname;
        
        public unsafe ServerSideBridge( void* bridge, string servername)
        {
            bridgehandle = bridge;
            sname = servername;
        }

        public void Await()
        {
            unsafe
            {
                UInt32 e = 0;
                Imports.ipcbAwaitClient(bridgehandle, &e);
                if( e != 0)
                {
                    throw new Exception(Imports.ipcbResolveErrorCode(e));
                }
            }
        }

        public void Dispose()
        {
            unsafe
            {
                Imports.ipcbCloseBridge(bridgehandle, Side.Server);
            }
        }

        public uint GetBufferSize()
        {
            unsafe
            {
                return Imports.ipcbGetSharedMemorySize(bridgehandle);
            }
        }

        public string getServerName()
        {
            return sname;
        }

        public bool isConnected()
        {
            unsafe
            {
                return Imports.ipcbIsClientConnectionOpen(bridgehandle);
            }
        }

        public uint Read(uint toread, byte[] buffer, uint bridgeoffset)
        {
            unsafe
            {
                return Imports.ipcbReadFromClient(bridgehandle, bridgeoffset, toread, buffer);
            }
        }

        public uint Read(uint toread, byte[] buffer) => Read(toread, buffer, 0);

        public void Signal()
        {
            unsafe
            {
                UInt32 e = 0;
                Imports.ipcbSignalClient(bridgehandle, &e);
                if( e != 0)
                {
                    throw new Exception(Imports.ipcbResolveErrorCode(e));
                }
            }
        }

        public uint Write(byte[] buffer, uint bridgeoffset)
        {
            unsafe
            {
                return Imports.ipcbWriteToClient(bridgehandle, bridgeoffset, buffer);
            }
        }

        public uint Write(byte[] buffer) => Write(buffer, 0);

    } //End of class

} //End of namespace

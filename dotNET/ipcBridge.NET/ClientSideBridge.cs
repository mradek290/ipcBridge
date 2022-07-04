using System;

namespace ipcBridge.NET
{
    internal class ClientSideBridge : IBridge
    {
        internal unsafe void* bridgehandle;
        private string sname;

        public unsafe ClientSideBridge( void* bridge, string servername)
        {
            bridgehandle = bridge;
            sname = servername;
        }

        public uint GetBufferSize()
        {
            unsafe
            {
                return Imports.ipcbGetSharedMemorySize(bridgehandle);
            }
        }

        public void Dispose()
        {
            unsafe
            {
                Imports.ipcbCloseBridge(bridgehandle, Side.Client);
            }
        }

        public void Await()
        {
            unsafe
            {
                UInt32 e = 0;
                Imports.ipcbAwaitServer(bridgehandle, &e);
                if( e != 0)
                {
                    throw new Exception(Imports.ipcbResolveErrorCode(e));
                }
            }
        }

        public void Signal()
        {
            unsafe
            {
                UInt32 e = 0;
                Imports.ipcbSignalServer(bridgehandle, &e);
                if( e != 0)
                {
                    throw new Exception(Imports.ipcbResolveErrorCode(e));
                }
            }
        }

        public uint Read( uint toread, byte[] buffer, uint bridgeoffset)
        {
            unsafe
            {
                return Imports.ipcbReadFromServer(bridgehandle, bridgeoffset, toread, buffer);
            }
        }

        public uint Read(uint toread, byte[] buffer) => Read(toread, buffer, 0);

        public uint Write(byte[] buffer, uint bridgeoffset)
        {
            unsafe
            {
                return Imports.ipcbWriteToServer(bridgehandle, bridgeoffset, buffer);
            }
        }

        public uint Write(byte[] buffer) => Write(buffer, 0);

        public bool isConnected()
        {
            unsafe
            {
                return Imports.ipcbIsServerConnectionOpen(bridgehandle);
            }
        }

        public string getServerName()
        {
            return sname;
        }

    } //End of class

} //End of namespace

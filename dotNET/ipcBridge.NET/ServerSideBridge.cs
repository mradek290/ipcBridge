using System;

namespace ipcBridge.NET
{
    internal class ServerSideBridge : IBridge
    {
        internal unsafe void* bridgehandle;
        
        public unsafe ServerSideBridge( void* bridge)
        {
            bridgehandle = bridge;
        }

        public void Await()
        {
            throw new NotImplementedException();
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

        public bool isConnected()
        {
            throw new NotImplementedException();
        }

        public uint Read(byte[] buffer, uint bridgeoffset)
        {
            throw new NotImplementedException();
        }

        public uint Read(byte[] buffer) => Read(buffer, 0);

        public void Signal()
        {
            throw new NotImplementedException();
        }

        public uint Write(byte[] buffer, uint bridgeoffset)
        {
            throw new NotImplementedException();
        }

        public uint Write(byte[] buffer) => Write(buffer, 0);

    } //End of class

} //End of namespace

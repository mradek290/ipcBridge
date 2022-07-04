using System;

namespace ipcBridge.NET
{
    internal class ClientSideBridge : IBridge
    {
        internal unsafe void* bridgehandle;

        public unsafe ClientSideBridge( void* bridge)
        {
            bridgehandle = bridge;
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
            throw new NotImplementedException();
        }

        public void Signal()
        {
            throw new NotImplementedException();
        }

        public uint Read(byte[] buffer, uint bridgeoffset)
        {
            throw new NotImplementedException();
        }

        public uint Read(byte[] buffer) => Read(buffer, 0);

        public uint Write(byte[] buffer, uint bridgeoffset)
        {
            throw new NotImplementedException();
        }

        public uint Write(byte[] buffer) => Write(buffer, 0);

        public bool isConnected()
        {
            throw new NotImplementedException();
        }

    } //End of class

} //End of namespace

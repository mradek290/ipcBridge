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

        public void Dispose()
        {
            throw new NotImplementedException();
        }
    } //End of class

} //End of namespace

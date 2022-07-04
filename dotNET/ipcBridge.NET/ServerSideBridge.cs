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

        public void Dispose()
        {
            throw new NotImplementedException();
        }

    } //End of class

} //End of namespace

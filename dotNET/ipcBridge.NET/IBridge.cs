using System;


namespace ipcBridge.NET
{
    public interface IBridge : IDisposable
    {
        uint GetBufferSize();
        void Await();
        void Signal();

        uint Read(uint toread, byte[] buffer, uint bridgeoffset);
        uint Read(uint toread, byte[] buffer);

        uint Write(byte[] buffer, uint bridgeoffset);
        uint Write(byte[] buffer);

        bool isConnected();

        string getServerName();

    } //End of interface

} //End of namespace

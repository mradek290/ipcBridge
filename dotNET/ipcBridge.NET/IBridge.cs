using System;


namespace ipcBridge.NET
{
    public interface IBridge : IDisposable
    {
        uint GetBufferSize();
        void Await();
        void Signal();

        uint Read(byte[] buffer, uint bridgeoffset);
        uint Read(byte[] buffer);

        uint Write(byte[] buffer, uint bridgeoffset);
        uint Write(byte[] buffer);

        bool isConnected();

    } //End of interface

} //End of namespace

using System;
using ipcBridge.NET;

namespace DemoClient
{
    class Entrypoint
    {
        static void Main(string[] args)
        {
            var connection = BridgeServer.Connect("mar");

        }
    }
}

using System;
using ipcBridge.NET;

namespace DemoServer
{
    class Entrypoint
    {
        static void Main(string[] args)
        {
            var server = new BridgeServer("mar");
            var connection = server.AwaitConnection();
            
        }
    }
}

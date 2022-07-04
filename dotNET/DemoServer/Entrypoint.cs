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
            connection.Await();

            var buffer = new byte[1000];
            connection.Read(buffer);
            string message = BitConverter.ToString(buffer);

            Console.WriteLine(message);

            server.Dispose();
            connection.Dispose();
            Console.WriteLine("Exiting");
        }
    }
}
